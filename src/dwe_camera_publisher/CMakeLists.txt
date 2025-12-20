cmake_minimum_required(VERSION 3.0.2)
project(dwe_camera_publisher)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(catkin REQUIRED COMPONENTS
  roscpp
  sensor_msgs
  std_msgs
  image_transport
  cv_bridge
)

find_package(OpenCV REQUIRED)
find_package(JPEG REQUIRED)
find_package(Threads REQUIRED)

set(BASECAMERA_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/../../BaseCameraAPI/src)
set(BASECAMERA_INCLUDE_DIR ${BASECAMERA_ROOT}/include)

set(BASECAMERA_SOURCES
  ${BASECAMERA_ROOT}/JPEGDecoder.cpp
  ${BASECAMERA_ROOT}/SynchronizedCamera.cpp
  ${BASECAMERA_ROOT}/V4L2Camera.cpp
)

catkin_package(
  CATKIN_DEPENDS roscpp sensor_msgs std_msgs image_transport cv_bridge
  DEPENDS OpenCV JPEG
  LIBRARIES dwe_camera_lib
)

add_library(dwe_camera_lib
  ${BASECAMERA_SOURCES}
)

target_include_directories(dwe_camera_lib
  PUBLIC
    ${BASECAMERA_INCLUDE_DIR}
    ${BASECAMERA_ROOT}
)

target_link_libraries(dwe_camera_lib
  PUBLIC
    JPEG::JPEG
    Threads::Threads
)

add_executable(test_camera_node
  src/test_camera_node.cpp
)

add_dependencies(test_camera_node
  ${catkin_EXPORTED_TARGETS}
)

target_include_directories(test_camera_node
  PRIVATE
    ${BASECAMERA_INCLUDE_DIR}
    ${BASECAMERA_ROOT}
    ${catkin_INCLUDE_DIRS}
    ${OpenCV_INCLUDE_DIRS}
)

target_link_libraries(test_camera_node
  PRIVATE
    dwe_camera_lib
    ${catkin_LIBRARIES}
    ${OpenCV_LIBRARIES}
)

install(TARGETS dwe_camera_lib test_camera_node
  ARCHIVE DESTINATION ${CATKIN_PACKAGE_LIB_DESTINATION}
  LIBRARY DESTINATION ${CATKIN_PACKAGE_LIB_DESTINATION}
  RUNTIME DESTINATION ${CATKIN_PACKAGE_BIN_DESTINATION}
)
