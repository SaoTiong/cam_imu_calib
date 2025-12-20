cmake_minimum_required(VERSION 3.0.2)
project(icm20948_serial)

find_package(catkin REQUIRED COMPONENTS
  rospy
  sensor_msgs
  std_msgs
)

catkin_package()

catkin_install_python(PROGRAMS
  scripts/icm20948_serial_node.py
  DESTINATION ${CATKIN_PACKAGE_BIN_DESTINATION}
)
