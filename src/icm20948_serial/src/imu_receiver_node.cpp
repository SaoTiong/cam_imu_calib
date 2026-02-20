#include <serialib.h>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <string>
#include <vector>

#include <ros/ros.h>
#include <sensor_msgs/Imu.h>

static constexpr uint8_t SYNC1 = 0xAA;
static constexpr uint8_t SYNC2 = 0x55;
static constexpr size_t PAYLOAD_SIZE = 32; // 4+4+6*4
static constexpr size_t PACKET_SIZE = 2 + PAYLOAD_SIZE;

#pragma pack(push, 1)
struct ImuPayload {
  uint32_t seq;
  uint32_t t_us;
  float ax, ay, az;
  float gx, gy, gz;
};
#pragma pack(pop)

static_assert(sizeof(ImuPayload) == PAYLOAD_SIZE, "Unexpected payload size");

int main(int argc, char** argv) {
  ros::init(argc, argv, "icm20948_serial_cpp_node");
  ros::NodeHandle nh;
  ros::NodeHandle pnh("~");

  std::string port = "/dev/ttyACM0";
  int baud = 115200;
  std::string topic = "/icm20948/imu";
  std::string frame_id = "icm20948_link";

  pnh.param<std::string>("port", port, port);
  pnh.param<int>("baud_rate", baud, baud);
  pnh.param<std::string>("topic", topic, topic);
  pnh.param<std::string>("frame_id", frame_id, frame_id);

  if (argc >= 2) port = argv[1];
  if (argc >= 3) baud = static_cast<int>(std::stoul(argv[2]));

  serial::Serial ser(port, static_cast<uint32_t>(baud),
                     serial::Timeout::simpleTimeout(10));
  if (!ser.isOpen()) {
    std::cerr << "Failed to open " << port << "\n";
    return 1;
  }

  ROS_INFO_STREAM("Reading IMU from " << port << " @ " << baud << " baud");

  ros::Publisher pub = nh.advertise<sensor_msgs::Imu>(topic, 200);

  std::vector<uint8_t> buf;
  buf.reserve(4096);

  while (ros::ok()) {
    std::vector<uint8_t> chunk;
    size_t n = ser.read(chunk, 1024);
    if (n == 0) {
      ros::spinOnce();
      continue;
    }
    buf.insert(buf.end(), chunk.begin(), chunk.end());

    while (buf.size() >= PACKET_SIZE && ros::ok()) {
      size_t i = 0;
      while (i + 1 < buf.size() && !(buf[i] == SYNC1 && buf[i + 1] == SYNC2)) i++;
      if (i > 0) buf.erase(buf.begin(), buf.begin() + i);
      if (buf.size() < PACKET_SIZE) break;

      ImuPayload p{};
      std::memcpy(&p, &buf[2], sizeof(ImuPayload));
      buf.erase(buf.begin(), buf.begin() + PACKET_SIZE);

      sensor_msgs::Imu msg;
      msg.header.stamp = ros::Time::now();
      msg.header.frame_id = frame_id;

      msg.linear_acceleration.x = p.ax;
      msg.linear_acceleration.y = p.ay;
      msg.linear_acceleration.z = p.az;

      msg.angular_velocity.x = p.gx;
      msg.angular_velocity.y = p.gy;
      msg.angular_velocity.z = p.gz;

      msg.orientation_covariance[0] = -1.0;

      pub.publish(msg);
    }

    ros::spinOnce();
  }
}
