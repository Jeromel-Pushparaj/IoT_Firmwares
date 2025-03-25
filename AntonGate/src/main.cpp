#include <Arduino.h>

#define PIR_SENSOR_PIN 13
#define DAY_NIGHT_SENSOR_PIN 12
#define WIFI_MODULE_PIN 14
#define RELAY_PIN 27

void setup() {
  pinMode(PIR_SENSOR_PIN, INPUT_PULLDOWN);
  pinMode(DAY_NIGHT_SENSOR_PIN, INPUT_PULLDOWN);
  pinMode(WIFI_MODULE_PIN, INPUT_PULLDOWN);
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, LOW);  // Ensure relay is OFF at startup
  Serial.begin(921600);
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