#include <Arduino.h>
int servoPin = 13; // PWM pin
const int freq = 50; // 50Hz for servo
const int ledChannel = 0;
const int resolution = 16; // 16-bit resolution

void setup() {
  ledcSetup(ledChannel, freq, resolution);
  ledcAttachPin(servoPin, ledChannel);
}

void setServoAngle(int angle) {
  // Convert angle (0-180) to duty cycle (500-2500 microseconds)
  int duty = map(angle, 0, 180, 1638, 8192); // 16-bit duty cycle
  ledcWrite(ledChannel, duty);
}

void loop() {
  for (int angle = 100; angle <= 130; angle++) {
    setServoAngle(angle);
  }
  delay(1000);
  for (int angle = 130; angle >= 100; angle--) {
    setServoAngle(angle);
  }
  delay(1000);
}
                                                                                                     