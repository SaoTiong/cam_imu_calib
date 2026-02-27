#include <ros/ros.h>
#include <sensor_msgs/Imu.h>
#include "imu_receiver.h"


ros::Time MicrosToRosTime(uint64_t us) {
  ros::Time t;
  t.sec = static_cast<uint32_t>(us / 1000000ULL);
  t.nsec = static_cast<uint32_t>((us % 1000000ULL) * 1000ULL);
  return t;
}


int main(int argc, char** argv) {
  ros::init(argc, argv, "imu_receiver_node");
  ros::NodeHandle nh;
  ros::NodeHandle pnh("~");

  std::string device = "/dev/ttyACM0";
  int baud = 115200;
  std::string frame_id = "imu_link";
  std::string topic = "icm20948/imu";
  int queue_size = 20;
  int read_timeout_ms = 10;

  pnh.param<std::string>("device", device, device);
  pnh.param<int>("baud", baud, baud);
  pnh.param<std::string>("frame_id", frame_id, frame_id);
  pnh.param<std::string>("topic", topic, topic);
  pnh.param<int>("queue_size", queue_size, queue_size);
  pnh.param<int>("read_timeout_ms", read_timeout_ms, read_timeout_ms);

  ros::Publisher pub = nh.advertise<sensor_msgs::Imu>(topic, queue_size);

  ImuReceiver receiver(device, baud);
  if (!receiver.Open()) {
    ROS_FATAL_STREAM("Failed to start IMU receiver on " << device << " @ " << baud);
    return 1;
  }

  ROS_INFO_STREAM("IMU receiver started: device=" << device << " baud=" << baud
                  << " topic=" << topic);

  while (ros::ok()) {
    Out output{};
    if (!receiver.ReadSample(&output,
                             static_cast<unsigned int>(read_timeout_ms))) {
      ros::spinOnce();
      continue;
    }

    sensor_msgs::Imu msg;
    msg.header.frame_id = frame_id;
    msg.header.stamp = MicrosToRosTime(output.correct_t);

    msg.orientation_covariance[0] = -1.0;

    msg.linear_acceleration.x = output.sample.ax;
    msg.linear_acceleration.y = output.sample.ay;
    msg.linear_acceleration.z = output.sample.az;

    msg.angular_velocity.x = output.sample.gx;
    msg.angular_velocity.y = output.sample.gy;
    msg.angular_velocity.z = output.sample.gz;

    pub.publish(msg);
    ros::spinOnce();
  }

  receiver.Close();
  return 0;
}