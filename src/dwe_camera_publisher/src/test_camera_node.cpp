#include "DWECamera/SynchronizedCamera.h"
#include "DWECamera/JPEGDecoder.h"

#include <ros/ros.h>
#include <image_transport/image_transport.h>
#include <sensor_msgs/Image.h>
#include <sensor_msgs/image_encodings.h>
#include <cv_bridge/cv_bridge.h>

#include <opencv2/opencv.hpp>

#include <csignal>
#include <chrono>
#include <iomanip>
#include <iostream>
#include <memory>
#include <thread>
#include <vector>
#include <linux/videodev2.h>

namespace
{
volatile sig_atomic_t running = 1;

void signalHandler(int signum)
{
  (void)signum;
  running = 0;
}

std::vector<std::string> getDefaultCameraPaths()
{
  return {"/dev/video2", "/dev/video4"};
}

} // namespace

int main(int argc, char **argv)
{
  ros::init(argc, argv, "test_camera_node");
  ros::NodeHandle nh;
  ros::NodeHandle pnh("~");

  signal(SIGINT, signalHandler);
  signal(SIGTERM, signalHandler);

  std::vector<std::string> camera_paths;
  if (!pnh.getParam("camera_paths", camera_paths))
  {
    camera_paths = getDefaultCameraPaths();
  }

  if (camera_paths.empty())
  {
    ROS_ERROR("No camera paths specified.");
    return 1;
  }

  int width = 1600;
  int height = 1200;
  int fps = 60;
  bool show_preview = true;
  std::string frame_id_base = "dwe_camera";

  pnh.param("width", width, width);
  pnh.param("height", height, height);
  pnh.param("fps", fps, fps);
  pnh.param("show_preview", show_preview, show_preview);
  pnh.param("frame_id_base", frame_id_base, frame_id_base);

  ROS_INFO_STREAM("Opening " << camera_paths.size() << " synchronized cameras");
  for (std::size_t i = 0; i < camera_paths.size(); ++i)
  {
    ROS_INFO_STREAM("  Camera " << i << ": " << camera_paths[i]);
  }
  ROS_INFO_STREAM("Resolution: " << width << "x" << height << " @ " << fps << " FPS");

  try
  {
    std::vector<std::shared_ptr<dwe::V4L2Camera>> cameras;
    cameras.reserve(camera_paths.size());
    for (const auto &path : camera_paths)
    {
      cameras.push_back(std::make_shared<dwe::V4L2Camera>(path, width, height, fps, V4L2_PIX_FMT_MJPEG));
    }

    dwe::SynchronizedCamera sync_camera(cameras);
    sync_camera.start();

    image_transport::ImageTransport it(nh);
    std::vector<image_transport::Publisher> image_pubs;
    image_pubs.reserve(cameras.size());
    for (std::size_t i = 0; i < cameras.size(); ++i)
    {
      std::string topic_name = "camera" + std::to_string(i) + "/image_raw";
      image_pubs.emplace_back(it.advertise(topic_name, 1));
      ROS_INFO_STREAM("Publishing camera " << i << " frames on topic: " << topic_name);
    }

    DWECamera::JPEGDecoder decoder;

    const std::string window_name = "Synchronized Cameras";
    if (show_preview)
    {
      cv::namedWindow(window_name, cv::WINDOW_NORMAL);
      cv::resizeWindow(window_name, 1280, 960);
    }

    int frame_count = 0;
    auto last_fps_time = std::chrono::steady_clock::now();

    while (ros::ok() && running)
    {
      std::vector<std::unique_ptr<CopiedFrame>> frames;
      bool success = sync_camera.grab(frames);

      if (!success || frames.empty())
      {
        ros::spinOnce();
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        continue;
      }

      std::vector<cv::Mat> decoded_frames;
      decoded_frames.reserve(frames.size());
      ros::Time stamp = ros::Time::now();

      for (std::size_t i = 0; i < frames.size(); ++i)
      {
        uint32_t decoded_width = 0, decoded_height = 0, channels = 0;
        std::vector<uint8_t> rgb_data = decoder.decode(
            frames[i]->data,
            frames[i]->size,
            decoded_width,
            decoded_height,
            channels);

        if (rgb_data.empty())
        {
          ROS_WARN_STREAM("Failed to decode frame from camera " << i << ": " << decoder.getLastError());
          decoded_frames.emplace_back();
          continue;
        }

        cv::Mat img_rgb(decoded_height, decoded_width, CV_8UC3, rgb_data.data());
        cv::Mat img_bgr;
        cv::cvtColor(img_rgb, img_bgr, cv::COLOR_RGB2BGR);
        decoded_frames.push_back(img_bgr);

        if (i < image_pubs.size())
        {
          cv_bridge::CvImage cv_image;
          cv_image.header.stamp = stamp;
          cv_image.header.frame_id = frame_id_base + "_" + std::to_string(i);
          cv_image.encoding = sensor_msgs::image_encodings::BGR8;
          cv_image.image = img_bgr;
          image_pubs[i].publish(cv_image.toImageMsg());
        }
      }

      if (show_preview && decoded_frames.size() >= 2 && !decoded_frames[0].empty() && !decoded_frames[1].empty())
      {
        cv::Mat grid;
        cv::hconcat(decoded_frames[0], decoded_frames[1], grid);
        cv::imshow(window_name, grid);
        int key = cv::waitKey(1);
        if (key == 'q' || key == 27)
        {
          running = 0;
        }
      }

      frame_count++;
      auto current_time = std::chrono::steady_clock::now();
      auto elapsed_ms = std::chrono::duration_cast<std::chrono::milliseconds>(current_time - last_fps_time).count();
      if (elapsed_ms >= 1000)
      {
        double current_fps = (frame_count * 1000.0) / elapsed_ms;
        ROS_INFO_STREAM("Current FPS: " << std::fixed << std::setprecision(2) << current_fps);
        frame_count = 0;
        last_fps_time = current_time;
      }

      ros::spinOnce();
    }

    sync_camera.stop();

    if (show_preview)
    {
      cv::destroyWindow(window_name);
    }

    ROS_INFO("Camera node shutting down.");
    return 0;
  }
  catch (const std::exception &ex)
  {
    ROS_ERROR_STREAM("Error in test_camera_node: " << ex.what());
    return 1;
  }
}
