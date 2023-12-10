#include <Wire.h>
#include <MPU6050.h>

MPU6050 mpu;

// Constants for the complementary filter
const float alpha = 0.98; // Weight for the gyroscope data
const float beta = 0.02;  // Weight for the accelerometer data

// Variables to store pitch angle
float pitchAngle = 0.0;

// Calibration factors
float accelAngleX = 0.0;
float gyroOffsetX = 0.0;

void setup() {
  Serial.begin(9600);
  
  Wire.begin();
  mpu.initialize();

  // Calibrate accelerometer and gyroscope
  calibrateSensors();
}

void loop() {
  // Read accelerometer and gyroscope data
  int16_t ax, ay, az, gx, gy, gz;
  mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
  
  // Remove biases and apply calibration
  float accelAngle = atan2(-ay - accelAngleX, az) * (180.0 / PI);
  float gyroRate = (gx - gyroOffsetX) / 131.0; // Adjust the division factor based on the sensitivity of your sensor

  // Calculate pitch angle using complementary filter
  pitchAngle = alpha * (pitchAngle + gyroRate * 0.01) + beta * accelAngle;
  
  Serial.print("Pitch: ");
  Serial.println(pitchAngle);
  
  delay(10); // Adjust the delay based on your application's requirements
}

void calibrateSensors() {
  const int numSamples = 1000;
  int32_t accelX = 0;
  int32_t gyroX = 0;
  int16_t ax, ay, az, gx, gy, gz;

  for (int i = 0; i < numSamples; ++i) {
    mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);

    accelX += ax;
    gyroX += gx;
    delay(1);
  }

  int16_t averageAz = accelX / numSamples;
  accelAngleX = atan2(-averageAz, az) * (180.0 / PI);
  gyroOffsetX = gyroX / numSamples;
}
