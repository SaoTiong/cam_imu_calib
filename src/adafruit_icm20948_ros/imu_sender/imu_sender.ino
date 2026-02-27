// Basic demo for accelerometer readings from Adafruit ICM20948

#include <Adafruit_ICM20X.h>
#include <Adafruit_ICM20948.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>

Adafruit_ICM20948 icm;


#define ICM20948_ADDR 0x69

#define REG_BANK_SEL   0x7F
#define INT_PIN_CFG    0x0F
#define INT_ENABLE_1   0x11
#define INTERRUPT_PIN 7


#pragma pack(push, 1)
struct ImuPayload {
  uint64_t t_us;
  float ax, ay, az;
  float gx, gy, gz;
};
#pragma pack(pop)


uint64_t micros64(uint32_t offset = 0) {
  static uint64_t now64 = 0;
  static bool flag = false;
  uint32_t now = micros();
  if ((now & 0x80000000) == 0) {
    if (flag) {
      flag = false;
      now64 += 0x100000000ULL;  //  1 << 32;
    }
  } else {
    flag = true;
  }
  if (offset == 0) return (now64 | now);
  return (now64 | now) + offset;
}


volatile bool imuDataReady = false;
volatile uint64_t irqTimestamp = 0;


void dataReadyISR() {
  irqTimestamp = micros64();
  imuDataReady = true;
}

void writeReg(uint8_t reg, uint8_t value) {
  Wire.beginTransmission(ICM20948_ADDR);
  Wire.write(reg);
  Wire.write(value);
  Wire.endTransmission();
}


void setBank(uint8_t bank) {
  writeReg(REG_BANK_SEL, (bank & 0x03) << 4);
}

// Enable the ICM20948 Raw Data Ready interrupt on INT1 pin
void enableINT() {
  setBank(0);
  writeReg(INT_PIN_CFG, 0x12);
  writeReg(INT_ENABLE_1, 0x01);
}

void setup(void) {
  Serial.begin(115200);
  while (!Serial) delay(10);

  // Serial.println("Initializing ICM20948...");
  

  if (!icm.begin_I2C(ICM20948_ADDR)) {
    // Serial.println("Failed to find ICM20948 chip. Check wiring and I2C address.");
    while (1) delay(10);
  }
  Serial.println("ICM20948 Found!");

  Wire.setClock(400000);

  icm.setAccelRange(ICM20948_ACCEL_RANGE_4_G);
  icm.setGyroRange(ICM20948_GYRO_RANGE_500_DPS);

  icm.setAccelRateDivisor(3);
  icm.setGyroRateDivisor(3);

  enableINT();

  pinMode(INTERRUPT_PIN, INPUT);

  attachInterrupt(digitalPinToInterrupt(INTERRUPT_PIN), dataReadyISR, RISING);

  sensors_event_t a, g, t, m;
  icm.getEvent(&a, &g, &t, &m);

  // Serial.println("Timestamp_us, Interval_us, ax, ay, az, gx, gy, gz");

}

void loop() {
  
  if (imuDataReady) {

    noInterrupts();
    uint64_t t_us = irqTimestamp;
    imuDataReady = false;
    interrupts();

    // setBank(0);

    sensors_event_t accel, gyro, mag, temp;
    icm.getEvent(&accel, &gyro, &temp, &mag);

    

    ImuPayload p;
    p.t_us = t_us;
    p.ax = accel.acceleration.x;
    p.ay = accel.acceleration.y;
    p.az = accel.acceleration.z;
    p.gx = gyro.gyro.x;
    p.gy = gyro.gyro.y;
    p.gz = gyro.gyro.z;

    

    // uint8_t packet[2 + sizeof(ImuPayload)];
    // packet[0] = 0xAA;
    // packet[1] = 0x55;
    // memcpy(&packet[2], &p, sizeof(ImuPayload));
    // Serial.write(packet, sizeof(packet));

    uint8_t packet[sizeof(ImuPayload)];
    memcpy(&packet[0], &p, sizeof(ImuPayload));
    Serial.write(packet, sizeof(packet));

    
  }

}
