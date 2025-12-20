#!/usr/bin/env python3
"""Publish calibrated ICM20948 IMU data read over a serial link."""

import argparse
import struct
from typing import List, Optional, Tuple

import rospy
from sensor_msgs.msg import Imu

try:
    import serial
except ImportError as exc:
    raise ImportError(
        "pyserial is required. Install it via 'sudo apt install python3-serial' or 'pip install pyserial'."
    ) from exc


PACKET_HEADER = 0xAA
PACKET_FOOTER = 0x55
PAYLOAD_FLOATS = 6
PAYLOAD_SIZE = PAYLOAD_FLOATS * 4
PACKET_SIZE = PAYLOAD_SIZE + 2


def _covariance(variance: float) -> List[float]:
    cov = [0.0] * 9
    if variance > 0.0:
        cov[0] = cov[4] = cov[8] = variance
    return cov


class SerialImuPublisher:
    """Subscribe to an Arduino serial stream and publish `sensor_msgs/Imu`."""

    def __init__(self) -> None:
        parser = argparse.ArgumentParser(add_help=False)
        parser.add_argument("__name", nargs="?")
        parser.add_argument("__log", nargs="?")
        parser.parse_known_args()

        rospy.init_node("icm20948_serial_node")

        port = rospy.get_param("~port", "/dev/ttyUSB1")
        baud_rate = rospy.get_param("~baud_rate", 115200)
        timeout = rospy.get_param("~timeout", 0.02)
        topic = rospy.get_param("~topic", "/icm20948/imu")
        self.frame_id = rospy.get_param("~frame_id", "icm20948_link")
        angular_var = rospy.get_param("~angular_velocity_variance", 0.0)
        accel_var = rospy.get_param("~linear_acceleration_variance", 0.0)
        self.use_binary = rospy.get_param("~binary_mode", True)

        self.angular_cov = _covariance(angular_var)
        self.accel_cov = _covariance(accel_var)

        try:
            self.serial = serial.Serial(port=port, baudrate=baud_rate, timeout=timeout)
        except serial.SerialException as exc:
            rospy.logfatal("Unable to open %s: %s", port, exc)
            raise

        self.publisher = rospy.Publisher(topic, Imu, queue_size=500)
        mode = "binary" if self.use_binary else "csv"
        rospy.loginfo(
            "Streaming ICM20948 data (%s) from %s to %s (baud=%d)", mode, port, topic, baud_rate
        )

        self._buffer = bytearray()

    def spin(self) -> None:
        rate = rospy.Rate(2000)
        while not rospy.is_shutdown():
            sample = self._read_binary_sample() if self.use_binary else self._read_csv_sample()
            if sample is None:
                rate.sleep()
                continue

            ax, ay, az, gx, gy, gz = sample
            msg = Imu()
            msg.header.stamp = rospy.Time.now()
            msg.header.frame_id = self.frame_id
            msg.orientation_covariance[0] = -1.0

            msg.angular_velocity.x = gx
            msg.angular_velocity.y = gy
            msg.angular_velocity.z = gz
            msg.angular_velocity_covariance = self.angular_cov

            msg.linear_acceleration.x = ax
            msg.linear_acceleration.y = ay
            msg.linear_acceleration.z = az
            msg.linear_acceleration_covariance = self.accel_cov

            self.publisher.publish(msg)
            rate.sleep()

    def _read_csv_sample(self) -> Optional[Tuple[float, float, float, float, float, float]]:
        try:
            line = self.serial.readline().decode("utf-8", errors="ignore").strip()
        except serial.SerialException as exc:
            rospy.logwarn_throttle(5.0, "Serial read failed: %s", exc)
            return None

        if not line:
            return None

        fields = [part.strip() for part in line.split(",")]
        if len(fields) < 6:
            rospy.logwarn_throttle(5.0, "Malformed sample: %s", line)
            return None

        try:
            return tuple(float(v) for v in fields[:6])  # type: ignore[return-value]
        except ValueError:
            rospy.logwarn_throttle(5.0, "Could not parse floats: %s", line)
            return None

    def _read_binary_sample(self) -> Optional[Tuple[float, float, float, float, float, float]]:
        try:
            chunk = self.serial.read(self.serial.in_waiting or PACKET_SIZE)
        except serial.SerialException as exc:
            rospy.logwarn_throttle(5.0, "Serial read failed: %s", exc)
            return None

        if chunk:
            self._buffer.extend(chunk)

        while len(self._buffer) >= PACKET_SIZE:
            header_index = self._buffer.find(bytes([PACKET_HEADER]))
            if header_index == -1:
                self._buffer.clear()
                break
            if header_index > 0:
                del self._buffer[:header_index]
                if len(self._buffer) < PACKET_SIZE:
                    break

            if len(self._buffer) < PACKET_SIZE:
                break

            footer_index = 1 + PAYLOAD_SIZE
            if self._buffer[footer_index] != PACKET_FOOTER:
                del self._buffer[0]
                continue

            payload = self._buffer[1:1 + PAYLOAD_SIZE]
            del self._buffer[:PACKET_SIZE]

            if len(payload) != PAYLOAD_SIZE:
                continue
            try:
                return struct.unpack("<6f", payload)
            except struct.error:
                rospy.logwarn_throttle(5.0, "Failed to unpack binary payload")
                continue

        return None


def main() -> None:
    try:
        SerialImuPublisher().spin()
    except rospy.ROSInterruptException:
        pass


if __name__ == "__main__":
    main()
