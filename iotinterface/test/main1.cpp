#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <DHT.h>

#define WIFI_SSID "Light"
#define WIFI_PASSWORD "12354768"

#define API_KEY "18e79dea75ec7cac5987a2176624e42b47ccc943f50638ec4e6d8fedd5c5ef3c3eb97c1de41752a419882981828d2b016f82881218c45c94b3b20f84991cc576"
#define DEVICE_KEY "c1885e604c094bf54e31177abdc0ed15fbf7b8cc3259b72dd9b7c8e1f39b8acd"

#define API_BASE "https://iotinterface.site/api/device/"
#define DEVICE_ENTRY API_BASE "deviceentry"
#define DEVICE_STATUS API_BASE "status"
#define DEVICE_UPDATE API_BASE "update"

#define DHTPIN 4
#define DHTTYPE DHT22

#define RELAY_TEMP 5
#define RELAY_LIGHT 21
#define BTN_INC 18
#define BTN_DEC 19
#define BTN_LIGHT 22

void deviceEntry();
void getDeviceState();
void updateDeviceState();
void updateDisplay(String content);
void updateIndicator();
void updateButton(int state);
void getslider();


unsigned long lastApiCall = 0;
const long apiInterval = 5000; // Call API every 5 seconds

DHT dht(DHTPIN, DHTTYPE);
float temp_threshold = 22.0;
bool relay_state = false;

void setup() {
    Serial.begin(115200);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    
    pinMode(RELAY_TEMP, OUTPUT);
    pinMode(RELAY_LIGHT, OUTPUT);
    pinMode(BTN_INC, INPUT_PULLUP);
    pinMode(BTN_DEC, INPUT_PULLUP);
    pinMode(BTN_LIGHT, INPUT_PULLUP);
    
    digitalWrite(RELAY_TEMP, LOW);
    digitalWrite(RELAY_LIGHT, LOW);

    Serial.print("Connecting to WiFi...");
    while (WiFi.status() != WL_CONNECTED) {
        Serial.print(".");
        delay(1000);
    }
    Serial.println("\nWiFi Connected!");

    deviceEntry();
}

void loop() {
    float temperature = dht.readTemperature();
    float humidity = dht.readHumidity();

    if (isnan(temperature) || isnan(humidity)) {
        Serial.println("Failed to read from DHT sensor!");
        return;
    }
    char content[100];
    snprintf(content, sizeof(content), "Temp: %.1f °C | Humidity: %.1f %% | Thresholding: %.1f\n", temperature , humidity, temp_threshold);

    // Relay 1 (Temp Control)
    if (temperature >= temp_threshold) {
        digitalWrite(RELAY_TEMP, HIGH);
        relay_state = true;
    } else {
        digitalWrite(RELAY_TEMP, LOW);
        relay_state = false;
    }
    Serial.printf("Relay State (Temp): %s\n", relay_state ? "ON" : "OFF");

    // Button Controls for Temp Threshold
    if (digitalRead(BTN_INC) == LOW) {
        temp_threshold += 1.0;
        Serial.printf("Threshold Increased: %.1f\n", temp_threshold);
        // delay(500);
    }
    if (digitalRead(BTN_DEC) == LOW) {
        temp_threshold -= 1.0;
        Serial.printf("Threshold Decreased: %.1f\n", temp_threshold);
        // delay(500);
    }

    // Relay 2 (Light Control via Button)
    if (digitalRead(BTN_LIGHT) == LOW && digitalRead(RELAY_LIGHT) == LOW) {
        digitalWrite(RELAY_LIGHT, HIGH);
        updateButton(1);
        Serial.println("Light ON");
    } else if(digitalRead(BTN_LIGHT) == LOW && digitalRead(RELAY_LIGHT) == HIGH){
        digitalWrite(RELAY_LIGHT, LOW);
        updateButton(0);
        Serial.println("Light OFF");
    }

    // Call API only if enough time has passed
    if (millis() - lastApiCall >= apiInterval) {
        lastApiCall = millis();
        getDeviceState();
        updateDeviceState();
        updateDisplay(content);
        getslider();
    }
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

void getDeviceState() {
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
        deserializeJson(jsonResponse, response);

        if (jsonResponse.containsKey("buttonstate")) {
            bool buttonState = jsonResponse["buttonstate"];
            digitalWrite(RELAY_LIGHT, buttonState ? HIGH : LOW);
            Serial.printf("Updated Light Relay via API: %s\n", buttonState ? "ON" : "OFF");
        }
    } else {
        Serial.printf("Failed to Get State: %s\n", http.errorToString(httpResponseCode).c_str());
    }
    http.end();
}

void updateDeviceState() {
    HTTPClient http;
    http.begin(DEVICE_UPDATE);
    http.addHeader("Authorization", API_KEY);
    http.addHeader("devicekey", DEVICE_KEY);
    http.addHeader("Content-Type", "application/json");

    StaticJsonDocument<200> jsonDoc;
    jsonDoc["indicator"] = relay_state ? 1 : 0;
    jsonDoc["slidervalue"] = (int)temp_threshold;
    jsonDoc["display"] = "HEllo From esp32";

    String requestBody;
    serializeJson(jsonDoc, requestBody);

    int httpResponseCode = http.POST(requestBody);
    if (httpResponseCode > 0) {
        Serial.printf("Device Update Success: %d\n", httpResponseCode);
    } else {
        Serial.printf("Device Update Failed: %s\n", http.errorToString(httpResponseCode).c_str());
    }
    http.end();
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
void getslider() {
    HTTPClient http;
    http.begin(DEVICE_STATUS);
    http.addHeader("Authorization", API_KEY);
    http.addHeader("devicekey", DEVICE_KEY);
    http.addHeader("Content-Type", "application/json");

    StaticJsonDocument<200> request;
    request["state"] = "slidervalue";
    String requestBody;
    serializeJson(request, requestBody);

    int httpResponseCode = http.POST(requestBody);
    if (httpResponseCode > 0) {
        String response = http.getString();
        Serial.printf("Device State Response: %s\n", response.c_str());

        StaticJsonDocument<200> jsonResponse;
        deserializeJson(jsonResponse, response);

        if (jsonResponse.containsKey("slidervalue")) {
            int slidervalue = jsonResponse["slidervalue"];
            temp_threshold = slidervalue;

        }
    } else {
        Serial.printf("Failed to Get State: %s\n", http.errorToString(httpResponseCode).c_str());
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
    String payload = "{\"indicator\": 1}";
    int httpResponseCode = http.POST(payload);
    if (httpResponseCode > 0) {
        Serial.printf("Device Indicator Update Success: %d | Response:  %s\n", httpResponseCode);
    } else {
        Serial.printf("Device Indicator Update Failed: %s\n", http.errorToString(httpResponseCode).c_str());
    }
    http.end();
}