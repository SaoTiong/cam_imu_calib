#include <linux/videodev2.h>

#include <algorithm>
#include <cstdint>
#include <memory>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

#include <ros/ros.h>
#include <sensor_msgs/Image.h>

#include "DWECamera/JPEGDecoder.h"
#include "DWECamera/SynchronizedCamera.h"

namespace {

std::string Trim(const std::string& input) {
  const auto begin = input.find_first_not_of(" \t");
  if (begin == std::string::npos) {
    return "";
  }
  const auto end = input.find_last_not_of(" \t");
  return input.substr(begin, end - begin + 1);
}

bool ReadCameraPaths(ros::NodeHandle& pnh, std::vector<std::string>* camera_paths) {
  if (camera_paths == nullptr) {
    return false;
  }

  std::string camera_paths_csv = "/dev/video2,/dev/video4";
  pnh.param<std::string>("camera_paths_csv", camera_paths_csv, camera_paths_csv);

  camera_paths->clear();
  std::stringstream ss(camera_paths_csv);
  std::string token;
  while (std::getline(ss, token, ',')) {
    token = Trim(token);
    if (!token.empty()) {
      camera_paths->push_back(token);
    }
  }

  if (camera_paths->empty()) {
    ROS_ERROR("~camera_paths_csv resolved to an empty camera list");
    return false;
  }

  return true;
}

sensor_msgs::Image BuildImageMsg(const std::unique_ptr<CopiedFrame>& frame,
                                 DWECamera::JPEGDecoder* decoder,
                                 const std::string& frame_id) {
  sensor_msgs::Image msg;

  uint32_t decoded_width = 0;
  uint32_t decoded_height = 0;
  uint32_t channels = 0;

  std::vector<uint8_t> rgb_data = decoder->decode(
      frame->data, frame->size, decoded_width, decoded_height, channels);

  if (rgb_data.empty() || channels != 3) {
    return msg;
  }

  // msg.header.stamp.fromNSec(frame->timestamp_us * 1000ULL);
  msg.header.stamp = ros::Time::now();
  msg.header.frame_id = frame_id;
  msg.height = decoded_height;
  msg.width = decoded_width;
  msg.encoding = "rgb8";
  msg.is_bigendian = 0;
  msg.step = decoded_width * channels;
  msg.data = std::move(rgb_data);
  return msg;
}

}  // namespace

int main(int argc, char** argv) {
  ros::init(argc, argv, "camera_publish_node");
  ros::NodeHandle nh;
  ros::NodeHandle pnh("~");

  std::vector<std::string> camera_paths;
  if (!ReadCameraPaths(pnh, &camera_paths)) {
    return 1;
  }

  int width = 1600;
  int height = 1200;
  int fps = 60;
  std::string frame_prefix = "camera";
  std::string topic_prefix = "cameras";
  int queue_size = 2;
  int grab_retry_sleep_ms = 5;

  pnh.param("width", width, width);
  pnh.param("height", height, height);
  pnh.param("fps", fps, fps);
  pnh.param("frame_prefix", frame_prefix, frame_prefix);
  pnh.param("topic_prefix", topic_prefix, topic_prefix);
  pnh.param("queue_size", queue_size, queue_size);
  pnh.param("grab_retry_sleep_ms", grab_retry_sleep_ms, grab_retry_sleep_ms);

  std::vector<ros::Publisher> publishers;
  publishers.reserve(camera_paths.size());
  for (size_t i = 0; i < camera_paths.size(); ++i) {
    const std::string topic =
        topic_prefix + "/camera" + std::to_string(i) + "/image_raw";
    publishers.push_back(nh.advertise<sensor_msgs::Image>(topic, queue_size));
    ROS_INFO_STREAM("Camera " << i << ": " << camera_paths[i]
                    << " -> topic " << topic);
  }

  try {
    std::vector<std::shared_ptr<dwe::V4L2Camera>> cameras;
    cameras.reserve(camera_paths.size());
    for (const auto& path : camera_paths) {
      cameras.push_back(std::make_shared<dwe::V4L2Camera>(path, width, height,fps, V4L2_PIX_FMT_MJPEG));
    }

    dwe::SynchronizedCamera sync_camera(cameras);
    DWECamera::JPEGDecoder decoder;

    sync_camera.start();
    ROS_INFO("Synchronized camera streaming started");

    while (ros::ok()) {
      std::vector<std::unique_ptr<CopiedFrame>> frames;
      const bool ok = sync_camera.grab(frames);
      if (!ok || frames.empty()) {
        ros::Duration(grab_retry_sleep_ms / 1000.0).sleep();
        ros::spinOnce();
        continue;
      }

      const size_t n = std::min(frames.size(), publishers.size());
      for (size_t i = 0; i < n; ++i) {
        const std::string frame_id = frame_prefix + std::to_string(i);
        sensor_msgs::Image msg = BuildImageMsg(frames[i], &decoder, frame_id);
        if (msg.data.empty()) {
          ROS_WARN_THROTTLE(1.0, "Failed to decode frame from camera %zu", i);
          continue;
        }
        publishers[i].publish(msg);
      }

      ros::spinOnce();
    }

    sync_camera.stop();
    ROS_INFO("Synchronized camera streaming stopped");
    return 0;
  } catch (const std::exception& e) {
    ROS_FATAL_STREAM("camera_publish_node failed: " << e.what());
    return 1;
  }
}
