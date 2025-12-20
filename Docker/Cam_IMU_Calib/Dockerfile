FROM osrf/ros:noetic-desktop-full

ENV DEBIAN_FRONTEND=noninteractive 

SHELL ["/bin/bash", "-c"]

# Union of dependencies required for IMU calibration workflows and Kalibr multi-camera calibration.
RUN apt-get update && apt-get install -y --no-install-recommends \
    git wget autoconf automake nano vim tmux v4l-utils\
    python3-dev python3-pip python3-scipy python3-matplotlib \
    python3-serial python3-catkin-tools python3-osrf-pycommon python3-vcstool \
    ipython3 python3-wxgtk4.0 python3-tk python3-igraph python3-pyx \
    build-essential \
    libyaml-cpp-dev \
    libeigen3-dev libboost-all-dev libsuitesparse-dev \
    libopencv-dev libpoco-dev libtbb-dev libblas-dev liblapack-dev libv4l-dev \
    doxygen \
    ros-noetic-tf2-geometry-msgs \
    ros-noetic-rosbag \
    && rm -rf /var/lib/apt/lists/*

# WORKDIR /calib_ws

# Source the ROS and workspace overlays by default.
RUN echo "source /opt/ros/noetic/setup.bash" >> /root/.bashrc

CMD ["bash"]
