FROM ros:noetic-ros-base

ENV DEBIAN_FRONTEND=noninteractive
SHELL ["/bin/bash", "-c"]

# Install tools needed to build catkin workspaces and interact with serial IMUs.
RUN apt-get update && \
    apt-get install -y --no-install-recommends \
        vim \
        tmux \
        libyaml-cpp-dev \
        python3-serial \
        python3-pip \
        python3-catkin-tools \
        build-essential \
        ros-noetic-tf2-geometry-msgs \
        ros-noetic-rosbag && \
    rm -rf /var/lib/apt/lists/*

# Create a catkin workspace inside the container.
WORKDIR /ws
# RUN mkdir -p src && \
#     source /opt/ros/noetic/setup.bash && \
#     catkin init

# # Copy the catkin workspace (this repo) into the container.
# COPY src /ws/src

# Build the workspace so the binaries/scripts are ready.
# RUN source /opt/ros/noetic/setup.bash && \
#     catkin config --extend /opt/ros/noetic && \
#     catkin build

# Source workspace overlay when the container runs.
RUN echo "source /opt/ros/noetic/setup.bash" >> /root/.bashrc 
    # echo "source /ws/devel/setup.bash" >> /root/.bashrc

CMD ["bash"]
