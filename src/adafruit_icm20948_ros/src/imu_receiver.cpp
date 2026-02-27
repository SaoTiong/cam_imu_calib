#include "imu_receiver.h"

#include <chrono>
#include <cstring>
#include <iostream>
#include <limits>
#include <string>
#include <thread>
#include <vector>

constexpr size_t kPacketSize = sizeof(ImuSample);

ImuReceiver::ImuReceiver(const std::string& device, int baud)
    : device_(device), baud_(baud), is_open_(false), time_sync_() {}

bool ImuReceiver::Open() {
  if (is_open_) {
    return true;
  }

  const char rc = ser_.openDevice(device_.c_str(), static_cast<unsigned int>(baud_));
  if (rc < 0) {
    std::cerr << "Failed to open serial device " << device_ << " at " << baud_
              << " baud, rc=" << static_cast<int>(rc) << std::endl;
    return false;
  }

  // Give MCU time to reboot after serial open toggles control lines.
  std::this_thread::sleep_for(std::chrono::milliseconds(1500));
  ser_.flushReceiver();
  is_open_ = true;
  return true;
}

void ImuReceiver::Close() {
  if (!is_open_) {
    return;
  }
  ser_.closeDevice();
  is_open_ = false;
}

bool ImuReceiver::IsOpen() const { return is_open_; }

bool ImuReceiver::ReadSample(Out* out, unsigned int timeout_ms) {
  if (out == nullptr || !is_open_) {
    return false;
  }

  uint64_t arrival_t = NowMicros();

  std::vector<uint8_t> chunk(kPacketSize);
  int n = ser_.readBytes(chunk.data(), static_cast<unsigned int>(kPacketSize),timeout_ms);
  if (n < static_cast<int>(kPacketSize)) {
    return false;
  }


  ImuSample sample{};
  std::memcpy(&sample, chunk.data(), sizeof(ImuSample));


  double arduino_t = static_cast<double>(sample.t_us);
  double host_t = static_cast<double>(arrival_t);
  double sync_t = time_sync_.online_sync(arduino_t, host_t);


  out->sample = sample;
  out->arrival_t = arrival_t;
  out->correct_t = static_cast<uint64_t>(sync_t);


  return true;
}

uint64_t ImuReceiver::NowMicros() {
  const auto now = std::chrono::system_clock::now().time_since_epoch();
  return static_cast<uint64_t>(
      std::chrono::duration_cast<std::chrono::microseconds>(now).count());
}


