#!/usr/bin/env python3
"""Publish calibrated ICM20948 IMU data read over a serial link with SMOOTH timestamps."""

import argparse
import struct
import threading
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

        port = rospy.get_param("~port", "/dev/ttyUSB0")
        baud_rate = rospy.get_param("~baud_rate", 115200)
        timeout = rospy.get_param("~timeout", 0.02)
        topic = rospy.get_param("~topic", "/icm20948/imu")
        self.frame_id = rospy.get_param("~frame_id", "icm20948_link")
        
        # --- NEW PARAMETER: IMU RATE ---
        self.imu_rate = rospy.get_param("~rate", 400.0) 
        self.expected_dt = 1.0 / self.imu_rate
        # -------------------------------

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
            "Streaming ICM20948 data (%s) from %s to %s (baud=%d, rate=%dHz)", 
            mode, port, topic, baud_rate, self.imu_rate
        )

        self._buffer = bytearray()
        self._sample_lock = threading.Lock()
        self._latest_sample: Optional[Tuple[float, float, float, float, float, float]] = None
        self._last_valid_sample: Optional[Tuple[float, float, float, float, float, float]] = None

        # timestamp smoothing variables
        self.last_stamp = None
        self.timer = rospy.Timer(rospy.Duration(self.expected_dt), self._timer_publish)
        self.serial_thread = threading.Thread(target=self._serial_worker, daemon=True)
        self.serial_thread.start()

    def spin(self) -> None:
        rospy.spin()

    def _serial_worker(self) -> None:
        loop_rate = rospy.Rate(2000)
        while not rospy.is_shutdown():
            sample = self._read_binary_sample() if self.use_binary else self._read_csv_sample()

            if sample is None:
                loop_rate.sleep()
                continue

            # Always keep only the newest sample so publishing side
            # never waits on stale data.
            with self._sample_lock:
                self._latest_sample = sample

    def _timer_publish(self, event: rospy.timer.TimerEvent) -> None:
        with self._sample_lock:
            sample = self._latest_sample
            if sample is not None:
                self._last_valid_sample = sample
                self._latest_sample = None
            else:
                sample = self._last_valid_sample

        if sample is None:
            rospy.logwarn_throttle(5.0, "No IMU samples received yet")
            return

        ax, ay, az, gx, gy, gz = sample
        msg = Imu()

        # --- FIX: SMOOTH TIMESTAMP LOGIC ---
        now = rospy.Time.now()
        if self.last_stamp is None:
            self.last_stamp = now
        else:
            self.last_stamp += rospy.Duration(self.expected_dt)
            if (now - self.last_stamp).to_sec() > 0.1 or (self.last_stamp - now).to_sec() > 0.1:
                rospy.logwarn("Timestamp sync reset due to lag/drift")
                self.last_stamp = now

        msg.header.stamp = self.last_stamp
        # -----------------------------------

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

    def _read_csv_sample(self) -> Optional[Tuple[float, float, float, float, float, float]]:
        try:
            if self.serial.in_waiting == 0: return None # Non-blocking check
            line = self.serial.readline().decode("utf-8", errors="ignore").strip()
        except serial.SerialException as exc:
            rospy.logwarn_throttle(5.0, "Serial read failed: %s", exc)
            return None

        if not line:
            return None

        fields = [part.strip() for part in line.split(",")]
        if len(fields) < 6:
            # excessive warning suppression
            return None

        try:
            return tuple(float(v) for v in fields[:6])  # type: ignore[return-value]
        except ValueError:
            return None

    def _read_binary_sample(self) -> Optional[Tuple[float, float, float, float, float, float]]:
        try:
            # Only read if data exists to avoid blocking
            waiting = self.serial.in_waiting
            if waiting < PACKET_SIZE and len(self._buffer) < PACKET_SIZE:
                return None
                
            chunk = self.serial.read(waiting or 1) # Read everything available
        except serial.SerialException as exc:
            rospy.logwarn_throttle(5.0, "Serial read failed: %s", exc)
            return None

        if chunk:
            self._buffer.extend(chunk)

        # Look for packets
        while len(self._buffer) >= PACKET_SIZE:
            header_index = self._buffer.find(bytes([PACKET_HEADER]))
            if header_index == -1:
                self._buffer.clear()
                break
            if header_index > 0:
                del self._buffer[:header_index]
                if len(self._buffer) < PACKET_SIZE:
                    break

            # Check footer
            if len(self._buffer) < PACKET_SIZE:
                break

            footer_index = 1 + PAYLOAD_SIZE
            if self._buffer[footer_index] != PACKET_FOOTER:
                del self._buffer[0] # Invalid packet, shift by 1 and retry
                continue

            # Extract Payload
            payload = self._buffer[1:1 + PAYLOAD_SIZE]
            
            # Remove this packet from buffer immediately so next loop finds the next one
            del self._buffer[:PACKET_SIZE] 

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
