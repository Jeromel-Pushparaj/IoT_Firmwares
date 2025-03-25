#include <Arduino.h>
#include <DHT.h>

#define DHTPIN 4         // GPIO for DHT sensor
#define DHTTYPE DHT22    // Change to DHT22 if using that sensor

#define RELAY_TEMP 5     // Relay 1: Temperature-controlled relay
#define RELAY_LIGHT 21   // Relay 2: Light control (turns on when button pressed)
#define BTN_INC 18       // Increase threshold button
#define BTN_DEC 19       // Decrease threshold button
#define BTN_LIGHT 22     // Button to turn on light relay

DHT dht(DHTPIN, DHTTYPE);

float temp_threshold = 30.0; // Default threshold for relay activation
bool relay_state = false;

void setup() {
    Serial.begin(115200);
    dht.begin();
    
    pinMode(RELAY_TEMP, OUTPUT);
    pinMode(RELAY_LIGHT, OUTPUT);
    pinMode(BTN_INC, INPUT_PULLUP);
    pinMode(BTN_DEC, INPUT_PULLUP);
    pinMode(BTN_LIGHT, INPUT_PULLUP);

    digitalWrite(RELAY_TEMP, LOW); // Initially keep temperature relay off
    digitalWrite(RELAY_LIGHT, LOW); // Initially keep light relay off
}

void loop() {
    // Read temperature and humidity
    float temperature = dht.readTemperature();
    float humidity = dht.readHumidity();

    if (isnan(temperature) || isnan(humidity)) {
        Serial.println("Failed to read from DHT sensor!");
        return;
    }

    Serial.print("Temperature: ");
    Serial.print(temperature);
    Serial.print(" °C  Humidity: ");
    Serial.print(humidity);
    Serial.println(" %");

    // Relay 1 (Temperature-based control)
    if (temperature >= temp_threshold) {
        digitalWrite(RELAY_TEMP, HIGH);
        relay_state = true;
    } else {
        digitalWrite(RELAY_TEMP, LOW);
        relay_state = false;
    }

    Serial.print("Relay State (Temperature Control): ");
    Serial.println(relay_state ? "ON" : "OFF");

    // Handle button press for threshold adjustment
    if (digitalRead(BTN_INC) == LOW) {
        temp_threshold += 1.0;
        Serial.print("Threshold Increased: ");
        Serial.println(temp_threshold);
        delay(500); // Debounce delay
    }
    
    if (digitalRead(BTN_DEC) == LOW) {
        temp_threshold -= 1.0;
        Serial.print("Threshold Decreased: ");
        Serial.println(temp_threshold);
        delay(500); // Debounce delay
    }

    // Relay 2 (Light Control - ON while button is pressed)
    if (digitalRead(BTN_LIGHT) == LOW && digitalRead(RELAY_LIGHT) == LOW) {
        digitalWrite(RELAY_LIGHT, HIGH); // Turn ON light relay
        Serial.println("Light Relay: ON");
    } else if(digitalRead(BTN_LIGHT) == LOW && digitalRead(RELAY_LIGHT) == HIGH){
        digitalWrite(RELAY_LIGHT, LOW); // Turn OFF light relay
        Serial.println("Light Relay: OFF");
    }

    delay(2000); // Delay to prevent excessive polling
}
