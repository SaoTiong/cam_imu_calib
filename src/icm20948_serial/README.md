# icm20948_serial

ROS package that bridges an Arduino-based ICM20948 sketch (either CSV or the preferred binary protocol) into `sensor_msgs/Imu` so you can log data for Allan variance analysis and Kalibr. The folder `firmware/icm20948_icm20948_binary.ino` contains an optimized sketch that streams fully calibrated samples as binary packets at ~480 Hz (header `0xAA`, payload of 6 floats little-endian, footer `0x55`) over a 921600 baud serial link. Flash it as-is or adapt it with your own calibration constants.

## Node

```
rosrun icm20948_serial icm20948_serial_node.py _port:=/dev/ttyACM0 _topic:=/icm20948/imu _frame_id:=icm20948_link
```

Parameters (private):
- `~port` (`/dev/ttyACM0`): Serial port of the Arduino.
- `~baud_rate` (`115200`, set to `921600` if you use the provided binary firmware)
- `~timeout` (`0.02` s)
- `~topic` (`/icm20948/imu`): Published `sensor_msgs/Imu` topic.
- `~frame_id` (`icm20948_link`)
- `~angular_velocity_variance`, `~linear_acceleration_variance`: Diagonal covariance entries (rad^2/s^2, m^2/s^4). Keep zero if unknown.
- `~binary_mode` (`true`): When true, parse the binary packets described above. Set to `false` to fall back to parsing ASCII CSV lines (`ax, ay, az, gx, gy, gz`).

The binary firmware + node combination comfortably keeps up with 480 Hz output (8× a 60 FPS camera). If you stick with CSV streaming, ensure your sketch throttles to a sustainable rate and set `~binary_mode:=false`.

## Allan variance workflow

1. Record at least 3 h of `/icm20948/imu` data at 480 Hz: `rosbag record -O icm20948_480hz.bag /icm20948/imu`
2. Run `allan_variance_ros` with `config/icm20948_serial.yaml` from this package to compute noise parameters.
3. The tool writes `imu.yaml`, directly usable by Kalibr.

A sample config is provided in `config/icm20948_serial.yaml`.
