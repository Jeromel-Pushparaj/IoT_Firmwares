#include <WiFi.h>

// The SSID name you want your ESP32 to broadcast
const char *ssid = "StaffRoom"; 
const char *password = "12345678"; // Optional password (min 8 chars)

void setup() {
  Serial.begin(115200);
  Serial.println("Starting ESP32 Wi-Fi Access Point");

  // Start Wi-Fi in Access Point mode
  bool result = WiFi.softAP(ssid, password);

  if (result) {
    Serial.println("✅ Access Point Started Successfully");
    Serial.print("SSID: ");
    Serial.println(ssid);
    Serial.print("IP Address: ");
    Serial.println(WiFi.softAPIP());
  } else {
    Serial.println("❌ Failed to start Access Point");
  }
}

void loop() {
  // Print number of connected devices every 5 seconds
  Serial.print("Connected Stations: ");
  Serial.println(WiFi.softAPgetStationNum());
  delay(5000);
}
