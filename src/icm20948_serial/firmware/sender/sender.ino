// Basic demo for accelerometer readings from Adafruit ICM20948

#include <Adafruit_ICM20X.h>
#include <Adafruit_ICM20948.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>

Adafruit_ICM20948 icm;
// uint16_t measurement_delay_us = 65535; // Delay between measurements for testing
// For SPI mode, we need a CS pin
#define ICM_CS 10
// For software-SPI mode we need SCK/MOSI/MISO pins
#define ICM_SCK 13
#define ICM_MISO 12
#define ICM_MOSI 11


// #pragma pack(push, 1)
// struct ImuPayload {
//   uint32_t seq;
//   uint32_t t_us;
//   float ax, ay, az;
//   float gx, gy, gz;
// };
// #pragma pack(pop)

#pragma pack(push, 1)
struct ImuPayload {
  uint32_t t_us;
  float ax, ay, az;
  float gx, gy, gz;
};
#pragma pack(pop)


//acc and gyro calib
static const float ACC_BIAS_MSS[3]   = {-0.04100180f, -0.09475613f,  0.10522700f}; //IMU wetlink -0.04100180f, -0.09475613f,  0.10522700f  || -0.10836936f, -0.14618062f, -0.24424600f
static const float ACC_SCALE[3]      = {0.99969568f,  1.00054180f,  0.98821163f}; //IMU wetlink  0.99969568f,  1.00054180f,  0.98821163f  || 1.00371170f, 0.99903278f, 0.98581686f

// Optional fallback gyro bias (rad/s). Prefer per-boot bias.
static const float GYRO_BIAS_RADS_FALLBACK[3] = {-0.01102138f, -0.01515991f,  0.01481001f}; //IMU wetlink -0.01102138f, -0.01515991f,  0.01481001f || -0.00458946f, 0.00458547f, -0.01216408f

static const uint32_t OUTPUT_RATE_HZ = 600;
static const uint32_t OUTPUT_PERIOD_US = 1000000UL / OUTPUT_RATE_HZ;

static uint32_t previous = 0;
// static uint32_t seq_counter = 0;


void setup(void) {
  Serial.begin(115200);
  while (!Serial)
    delay(10); // will pause Zero, Leonardo, etc until serial console opens

  // Serial.println("Adafruit ICM20948 test!");

  // Try to initialize!
  if (!icm.begin_I2C()) {
    // if (!icm.begin_SPI(ICM_CS)) {
    // if (!icm.begin_SPI(ICM_CS, ICM_SCK, ICM_MISO, ICM_MOSI)) {

    // Serial.println("Failed to find ICM20948 chip");
    while (1) {
      delay(10);
    }
  }
  // Serial.println("ICM20948 Found!");
  Wire.setClock(400000);

  //set acc and gyro read range
  icm.setAccelRange(ICM20948_ACCEL_RANGE_16_G);
  icm.setGyroRange(ICM20948_GYRO_RANGE_2000_DPS);

  //set acc and gyro sample rate
  icm.setAccelRateDivisor(2);
  //   uint16_t accel_divisor = icm.getAccelRateDivisor();
//   float accel_rate = 1125 / (1.0 + accel_divisor);
  icm.setGyroRateDivisor(2);

  // Disable mag updates since we only stream accel+gyro
  // icm.setMagDataRate(AK09916_MAG_DATARATE_SHUTDOWN);
//   uint8_t gyro_divisor = icm.getGyroRateDivisor();
//   float gyro_rate = 1100 / (1.0 + gyro_divisor);
  previous = micros(); 

}

void loop() {
  uint32_t now = micros();

  if ((int32_t)(now - previous) >= OUTPUT_PERIOD_US) {


    // previous += OUTPUT_PERIOD_US;
    previous = micros();

    // seq_counter++;
    //  /* Get a new normalized sensor event */
    sensors_event_t accel;
    sensors_event_t gyro;
    sensors_event_t mag;
    sensors_event_t temp;

    icm.getEvent(&accel, &gyro, &temp, &mag);

    float ax = (accel.acceleration.x - ACC_BIAS_MSS[0]) * ACC_SCALE[0];
    float ay = (accel.acceleration.y - ACC_BIAS_MSS[1]) * ACC_SCALE[1];
    float az = (accel.acceleration.z - ACC_BIAS_MSS[2]) * ACC_SCALE[2];

    float gx = gyro.gyro.x - GYRO_BIAS_RADS_FALLBACK[0];
    float gy = gyro.gyro.y - GYRO_BIAS_RADS_FALLBACK[1];
    float gz = gyro.gyro.z - GYRO_BIAS_RADS_FALLBACK[2];

    // struct ImuPayload {
    //   uint32_t seq;
    //   uint32_t t_us;
    //   float ax, ay, az;
    //   float gx, gy, gz;
    // };
    uint32_t t_us = micros();

    ImuPayload p{t_us, ax, ay, az, gx, gy, gz};

    uint8_t packet[2 + sizeof(ImuPayload)];
    packet[0] = 0xAA;
    packet[1] = 0x55;
    memcpy(&packet[2], &p, sizeof(ImuPayload));
    Serial.write(packet, sizeof(packet));
  }

// Serial.print(seq_counter); Serial.print(',');
// Serial.print(now); Serial.print(',');
// Serial.print(ax, 6); Serial.print(',');
// Serial.print(ay, 6); Serial.print(',');
// Serial.print(az, 6); Serial.print(',');
// Serial.print(gx, 6); Serial.print(',');
// Serial.print(gy, 6); Serial.print(',');
// Serial.print(gz, 6); Serial.print('\n');


}
