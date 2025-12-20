#include <Adafruit_ICM20X.h>
#include <Adafruit_ICM20948.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>

Adafruit_ICM20948 icm;

// --- CALIBRATION PARAMETERS ---
float A_B[3] = { -65.42, -160.26, -563.39 };
float A_inv[3][3] = {
  { 0.06137,  0.00012,  0.00018 },
  { 0.00012,  0.06096, -0.00007 },
  { 0.00018, -0.00007,  0.05982 }
};
float G_bias[3] = { 182.3, 134.2, -68.9 };

const float ACCEL_LSB_PER_G = 16384.0;
const float GRAVITY_MSS     = 9.80665;
const float GYRO_LSB_PER_DEG = 131.0;
#ifndef DEG_TO_RAD
#define DEG_TO_RAD 0.01745329251
#endif

// --- CONFIGURATION FOR 500 Hz ---
const uint32_t SERIAL_BAUD = 115200;  
const uint16_t LOOP_PERIOD_US = 2500; // 2000us = 500 Hz
const uint8_t PACKET_HEADER = 0xAA;
const uint8_t PACKET_FOOTER = 0x55;

struct __attribute__((packed)) ImuPacket {
  float ax, ay, az;
  float gx, gy, gz;
};

void setup(void) {
  Serial.begin(SERIAL_BAUD);
  while (!Serial) delay(10);

  // 1. Initialize Sensor
  if (!icm.begin_I2C()) {
    Serial.println("Failed to find ICM20948 chip");
    while (1) delay(10);
  }

  // 2. REQUIRED: Enable Fast I2C (400 kHz)
  // Without this, you cannot go faster than ~236 Hz.
  Wire.setClock(400000); 

  icm.setAccelRange(ICM20948_ACCEL_RANGE_2_G);
  icm.setGyroRange(ICM20948_GYRO_RANGE_250_DPS);
  
  // Disable dividers to ensure sensor is ready every 2ms
  icm.setAccelRateDivisor(0);
  icm.setGyroRateDivisor(0);
}

void loop() {
  static uint32_t last_time = 0;
  uint32_t now = micros();

  // Enforce precise 500 Hz timing
  if (now - last_time >= LOOP_PERIOD_US) {
    last_time = now;

    sensors_event_t accel, gyro, temp, mag;
    icm.getEvent(&accel, &gyro, &temp, &mag);

    // --- ACCEL CALIBRATION ---
    float raw_a[3] = {
      accel.acceleration.x * (float)(ACCEL_LSB_PER_G / GRAVITY_MSS),
      accel.acceleration.y * (float)(ACCEL_LSB_PER_G / GRAVITY_MSS),
      accel.acceleration.z * (float)(ACCEL_LSB_PER_G / GRAVITY_MSS)
    };

    float tmp_a[3];
    for (int i = 0; i < 3; i++) tmp_a[i] = raw_a[i] - A_B[i];

    float cal_a_lsb[3] = {0, 0, 0};
    for (int i = 0; i < 3; i++) {
      for (int j = 0; j < 3; j++) cal_a_lsb[i] += A_inv[i][j] * tmp_a[j];
    }

    float scale = GRAVITY_MSS / 1000.0;
    float cal_ax = cal_a_lsb[0] * scale;
    float cal_ay = cal_a_lsb[1] * scale;
    float cal_az = cal_a_lsb[2] * scale;

    // --- GYRO CALIBRATION ---
    float g_scale = (1.0 / GYRO_LSB_PER_DEG) * DEG_TO_RAD;
    float cal_gx = gyro.gyro.x - (G_bias[0] * g_scale);
    float cal_gy = gyro.gyro.y - (G_bias[1] * g_scale);
    float cal_gz = gyro.gyro.z - (G_bias[2] * g_scale);

    // --- SEND BINARY PACKET ---
    ImuPacket packet = { cal_ax, cal_ay, cal_az, cal_gx, cal_gy, cal_gz };
    
    Serial.write(PACKET_HEADER);
    Serial.write((uint8_t*)&packet, sizeof(packet));
    Serial.write(PACKET_FOOTER);

    // Serial.print(cal_ax, 4); Serial.print(",");
    // Serial.print(cal_ay, 4); Serial.print(",");
    // Serial.print(cal_az, 4); Serial.print(",");
    // Serial.print(cal_gx, 5); Serial.print(",");
    // Serial.print(cal_gy, 5); Serial.print(",");
    // Serial.println(cal_gz, 5);
  }
}