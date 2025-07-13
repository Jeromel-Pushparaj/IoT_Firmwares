#include <Arduino.h>

#define PIR_SENSOR_PIN 13
#define DAY_NIGHT_SENSOR_PIN 12
#define WIFI_MODULE_PIN 14
#define RELAY_PIN 27

void setup() {
  Serial.begin(921600);

  pinMode(PIR_SENSOR_PIN, INPUT);
  pinMode(DAY_NIGHT_SENSOR_PIN, INPUT);
  pinMode(WIFI_MODULE_PIN, INPUT);

  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, LOW);

  delay(1000);  // Wait for sensors to stabilize
}


void loop() {
  int pirState = digitalRead(PIR_SENSOR_PIN);
  int dayNightState = digitalRead(DAY_NIGHT_SENSOR_PIN);
  int wifiState = digitalRead(WIFI_MODULE_PIN);

  Serial.print("PIR: "); Serial.print(pirState);
  Serial.print(" | Day/Night: "); Serial.print(dayNightState);
  Serial.print(" | WiFi: "); Serial.print(wifiState);
  
  if (dayNightState == HIGH && (pirState == HIGH || wifiState == HIGH)) {
    digitalWrite(RELAY_PIN, LOW);
    Serial.println(" | RELAY: ON");
  } else {
    digitalWrite(RELAY_PIN, HIGH);
    Serial.println(" | RELAY: OFF");
  }
  
  delay(500);
}