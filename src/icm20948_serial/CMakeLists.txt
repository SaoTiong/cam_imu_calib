cmake_minimum_required(VERSION 3.0.2)
project(icm20948_serial)

find_package(catkin REQUIRED COMPONENTS
  roscpp
  rospy
  sensor_msgs
  std_msgs
)

catkin_package()

find_package(serial QUIET)
if(serial_FOUND)
  set(SERIAL_INCLUDE_DIRS ${serial_INCLUDE_DIRS})
  set(SERIAL_LIBRARIES ${serial_LIBRARIES})
else()
  find_path(SERIAL_INCLUDE_DIRS serial/serial.h
    PATHS /usr/local/include /usr/include
  )
  find_library(SERIAL_LIBRARIES serial
    PATHS /usr/local/lib /usr/lib
  )
endif()

add_executable(icm20948_serial_cpp_node
  src/imu_receiver_node.cpp
)

target_include_directories(icm20948_serial_cpp_node
  PRIVATE
    ${catkin_INCLUDE_DIRS}
    ${SERIAL_INCLUDE_DIRS}
)

target_link_libraries(icm20948_serial_cpp_node
  ${catkin_LIBRARIES}
  ${SERIAL_LIBRARIES}
)

catkin_install_python(PROGRAMS
  scripts/icm20948_serial_node.py
  DESTINATION ${CATKIN_PACKAGE_BIN_DESTINATION}
)

install(TARGETS icm20948_serial_cpp_node
  RUNTIME DESTINATION ${CATKIN_PACKAGE_BIN_DESTINATION}
)
