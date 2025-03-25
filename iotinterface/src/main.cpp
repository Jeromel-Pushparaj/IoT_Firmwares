#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

#define WIFI_SSID "Light"
#define WIFI_PASSWORD "12354768"

#define API_KEY "18e79dea75ec7cac5987a2176624e42b47ccc943f50638ec4e6d8fedd5c5ef3c3eb97c1de41752a419882981828d2b016f82881218c45c94b3b20f84991cc576"
#define DEVICE_KEY "aaf7f10adfad80432a125dfbee9c644f89a9f1f825ace50de6e53ebfd602476f"

#define API_BASE "https://iotinterface.site/api/device/"
#define DEVICE_ENTRY API_BASE "deviceentry"
#define DEVICE_STATUS API_BASE "status"
#define DEVICE_UPDATE API_BASE "update"



void deviceEntry();
void getButtonState();
void updateDisplay(String content);
void updateIndicator();
void updateButton(int state);

unsigned long lastUpdate = 0;
const int updateInterval = 5000; // Update every 5 seconds


// Pin Definitions
#define DOOR_SENSOR_PIN 19 //oor sensor input pin
#define RED_LED_PIN 22    // Red LED for open door
#define GREEN_LED_PIN 4   // Green LED for closed door
#define RELAY_PIN 5        // Relay to control light

void setup() {
    // Initialize Serial Monitor
    Serial.begin(115200);
    
    // Configure Pins
    pinMode(DOOR_SENSOR_PIN, INPUT_PULLUP); // Door sensor (HIGH = closed, LOW = open)
    pinMode(RED_LED_PIN, OUTPUT);
    pinMode(GREEN_LED_PIN, OUTPUT);
    pinMode(RELAY_PIN, OUTPUT);

    // Turn everything off initially
    digitalWrite(RED_LED_PIN, LOW);
    digitalWrite(GREEN_LED_PIN, LOW);
    digitalWrite(RELAY_PIN, HIGH); // Low-Level Trigger Relay: HIGH = OFF, LOW = ON

    // Connect to WiFi
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    Serial.print("Connecting to WiFi...");
    
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    
    Serial.println("\nWiFi Connected!");
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());
    deviceEntry();

}

void loop() {
    int doorState = digitalRead(DOOR_SENSOR_PIN); // Read sensor

    if (doorState == LOW){
        digitalWrite(RED_LED_PIN, LOW );
        digitalWrite(GREEN_LED_PIN,HIGH);
        digitalWrite(RELAY_PIN, HIGH); //rn ON light
        updateDisplay("Door Closed turned on");
        Serial.println("Door Closed RED ON");
    } else if(doorState == HIGH){
        digitalWrite(RED_LED_PIN,HIGH);
        digitalWrite(GREEN_LED_PIN, LOW );
        digitalWrite(RELAY_PIN, LOW ); // Turn OFF light
        updateDisplay("Door open light turned off");
        Serial.println("Door open green led on");
    }
    
    // Only update indicator and get button state every 5 seconds
    if (millis() - lastUpdate > updateInterval) {
        updateIndicator();
        delay(500);
        getButtonState();
        lastUpdate = millis();
    }
    // delay(500); // Small delay to avoid rapid switching
}

void deviceEntry() {
    HTTPClient http;
    http.begin(DEVICE_ENTRY);
    http.addHeader("Authorization", API_KEY);
    http.addHeader("devicekey", DEVICE_KEY);
    
    int httpResponseCode = http.POST("");
    if (httpResponseCode > 0) {
        Serial.printf("Device Entry Success: %d\n", httpResponseCode);
    } else {
        Serial.printf("Device Entry Failed: %s\n", http.errorToString(httpResponseCode).c_str());
    }
    http.end();
}

void getButtonState() {
    HTTPClient http;
    http.begin(DEVICE_STATUS);
    http.addHeader("Authorization", API_KEY);
    http.addHeader("devicekey", DEVICE_KEY);
    http.addHeader("Content-Type", "application/json");

    StaticJsonDocument<200> request;
    request["state"] = "buttonstate";
    String requestBody;
    serializeJson(request, requestBody);

    int httpResponseCode = http.POST(requestBody);
    if (httpResponseCode > 0) {
        String response = http.getString();
        Serial.printf("Device State Response: %s\n", response.c_str());

        StaticJsonDocument<200> jsonResponse;
        DeserializationError error = deserializeJson(jsonResponse, response);
        if (error) {
            Serial.print("JSON Parsing Failed: ");
            Serial.println(error.c_str());
        } else if (jsonResponse.containsKey("buttonstate")) {
            bool buttonState = jsonResponse["buttonstate"];
            digitalWrite(RELAY_PIN, buttonState ? LOW : HIGH);
            Serial.printf("Updated Light Relay via API: %s\n", buttonState ? "ON" : "OFF");
        }
    } else {
        Serial.printf("Failed to Get State: %s\n", http.errorToString(httpResponseCode).c_str());
    }
    http.end(); // Free resources
}



void updateDisplay(String content){
    HTTPClient http;
    http.begin(DEVICE_UPDATE);
    http.addHeader("Authorization", API_KEY);
    http.addHeader("devicekey", DEVICE_KEY);
    http.addHeader("Content-Type", "application/json");

    // String requestBody;
    StaticJsonDocument<200> doc;
    doc["display"] = content;
    String payload;
    serializeJson(doc, payload);



    int httpResponseCode = http.POST(payload);
    if (httpResponseCode > 0) {
        Serial.printf("Device Display Update Success: %d\n", httpResponseCode);
    } else {
        Serial.printf("Device Display Update Failed: %s\n", http.errorToString(httpResponseCode).c_str());
    }
    http.end();

}

void updateButton(int state){
    HTTPClient http;
    http.begin(DEVICE_UPDATE);
    http.addHeader("Authorization", API_KEY);
    http.addHeader("devicekey", DEVICE_KEY);
    http.addHeader("Content-Type", "application/json");

    // String requestBody;
    StaticJsonDocument<200> doc;
    doc["buttonstate"] = state;
    String payload;
    serializeJson(doc, payload);


    int httpResponseCode = http.POST(payload);
    if (httpResponseCode > 0) {
        Serial.printf("Device button Update Success: %d\n", httpResponseCode);
    } else {
        Serial.printf("Device button Update Failed: %s\n", http.errorToString(httpResponseCode).c_str());
    }
    http.end();

}

void updateIndicator(){
    HTTPClient http;
    http.begin(DEVICE_UPDATE);
    http.addHeader("Authorization", API_KEY);
    http.addHeader("devicekey", DEVICE_KEY);
    http.addHeader("Content-Type", "application/json");

    // String requestBody;
    // serializeJson(jsonDoc, requestBody);
    StaticJsonDocument<200> doc;
    doc["indicator"] = 1;
    String payload;
    serializeJson(doc, payload);
    int httpResponseCode = http.POST(payload);
    if (httpResponseCode > 0) {
        Serial.printf("Device Indicator Update Success: %d | Response:  %s\n", httpResponseCode);
    } else {
        Serial.printf("Device Indicator Update Failed: %s\n", http.errorToString(httpResponseCode).c_str());
    }
    http.end();
}