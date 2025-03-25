#include <Wire.h>
#include <Arduino.h>
// Define SMBus Address for the Battery
#define BATTERY_ADDRESS 0x0B  // Common address for smart batteries
#define PF_REGISTER 0x00     // Replace with actual register address for PF

void setup() {
    Serial.begin(115200);
    Wire.begin(21, 22); // SDA = GPIO21, SCL = GPIO22
    Serial.println("Initializing...");
}

void clearProtectionFlags() {
    Wire.beginTransmission(BATTERY_ADDRESS);
    Wire.write(PF_REGISTER); // Write the address of the PF register
    Wire.write(0x00);        // Write data to clear PF (assumes 0x00 clears the flag)
    if (Wire.endTransmission() == 0) {
        Serial.println("Protection flags cleared successfully.");
    } else {
        Serial.println("Failed to communicate with the battery.");
    }
}

void loop() {
    Serial.println("Attempting to clear protection flags...");
    clearProtectionFlags();
        delay(5000); // Retry every 5 seconds
}