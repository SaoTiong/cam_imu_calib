#ifndef ADAFRUIT_ICM20948_IMU_RECEIVER_H_
#define ADAFRUIT_ICM20948_IMU_RECEIVER_H_

#include <cstdint>
#include <string>
#include <vector>

#include "passive_time_sync.h"
#include "serialib.h"

#pragma pack(push, 1)
struct ImuSample {
  uint64_t t_us;
  float ax;
  float ay;
  float az;
  float gx;
  float gy;
  float gz;
};
#pragma pack(pop)

struct Out {
  ImuSample sample;
  uint64_t arrival_t;
  uint64_t correct_t;
};

class ImuReceiver {
 public:
  ImuReceiver(const std::string& device, int baud);

  bool Open();
  void Close();
  bool IsOpen() const;

  bool ReadSample(Out* out, unsigned int timeout_ms = 10);

 private:
  static uint64_t NowMicros();

  std::string device_;
  int baud_;
  bool is_open_;
  serialib ser_;
  passive_time_sync time_sync_;
};

#endif  // ADAFRUIT_ICM20948_IMU_RECEIVER_H_
