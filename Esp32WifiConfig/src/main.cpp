#include <Arduino.h>
#include <WiFiMulti.h>

#define WIFI_SSID "unkown_network!"
#define WIFI_PASSWORD "Bsnl@1234"

WiFiMulti wifimulti;

void setup() {
  Serial.begin(921600);
  pinMode(LED_BUILTIN, OUTPUT);
  wifimulti.addAP(WIFI_SSID, WIFI_PASSWORD);

  while (wifimulti.run() != WL_CONNECTED)
  {
    delay(100);
  }
  Serial.println("connected");
}

void loop() {
  digitalWrite(LED_BUILTIN, WiFi.status() == WL_CONNECTED);
}

