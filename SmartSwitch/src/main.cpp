#include <Arduino.h>
#include <WiFi.h>
#include <WiFiProv.h>
#include <RMaker.h>
#include <SimpleTimer.h>

#define DEFAULT_RELAY_MODE false

const char *service_name = "PROV_123456";
const char *pop = "12345678";

int servo_pin = 13;          // PWM pin for servo
const int freq = 50;         // 50Hz for servo
const int ledChannel = 0;
const int resolution = 16;   // 16-bit resolution
const int pcStatusPin = 12; // Pin to indicate PC status (optional) if pc turned on it give 5v


static uint8_t gpio_reset = 0;
static uint8_t relay_pin = 2;
static uint8_t manual_switch_pin = 4;

bool relay_state = DEFAULT_RELAY_MODE;
bool wifi_connected = false;
bool last_manual_switch_state = HIGH;

static Switch my_switch("light", &relay_pin);
static Switch my_pc("pc", &servo_pin);

void updateRelayState(bool state) {
    relay_state = state;
    digitalWrite(relay_pin, relay_state ? HIGH : LOW);
    my_switch.updateAndReportParam(ESP_RMAKER_DEF_POWER_NAME, relay_state);
    Serial.printf("Relay is now: %s\n", relay_state ? "ON" : "OFF");
}

void setServoAngle(int angle) {
    // Convert angle (0-180) to duty cycle (500-2500 microseconds)
    int duty = map(angle, 0, 180, 1638, 8192); // 16-bit duty cycle
    ledcWrite(ledChannel, duty);
}

void turnOnPC() {
    Serial.println("Turning ON PC...");
    for (int angle = 100; angle <= 135; angle++) {
        setServoAngle(angle);
        delay(15); // Smooth movement
    }
    delay(1500); // Hold button for 1 sec
    for (int angle = 135; angle >= 100; angle--) {
        setServoAngle(angle);
        delay(15);
    }
    bool pc_state = digitalRead(pcStatusPin);
    if (pc_state == HIGH) {
        Serial.println("PC is ON");
    } else {
        Serial.println("PC did not turn ON");
    }
    my_pc.updateAndReportParam(ESP_RMAKER_DEF_POWER_NAME, pc_state);
}

void write_callback(Device *device, Param *param, const param_val_t val, void *priv_data, write_ctx_t *ctx) {
    const char *device_name = device->getDeviceName();
    const char *param_name = param->getParamName();

    if (strcmp(device_name, "light") == 0 && strcmp(param_name, "Power") == 0) {
        updateRelayState(val.val.b);
    }
    
    if (strcmp(device_name, "pc") == 0 && strcmp(param_name, "Power") == 0) {
        if (val.val.b) {
            turnOnPC();
        } else {
            setServoAngle(100); // Reset servo position
            my_pc.updateAndReportParam(ESP_RMAKER_DEF_POWER_NAME, false);
            Serial.println("PC turned OFF");
        }
    }
}

void sysProvEvent(arduino_event_t *sys_event) {
    switch (sys_event->event_id) {
        case ARDUINO_EVENT_PROV_START:
            Serial.printf("Provisioning Started with service name \"%s\" and PoP \"%s\"\n", service_name, pop);
            printQR(service_name, pop, "ble");
            break;
        case ARDUINO_EVENT_WIFI_STA_CONNECTED:
            Serial.println("Connected to Wi-Fi!");
            wifi_connected = true;
            break;
        case ARDUINO_EVENT_PROV_CRED_RECV:
            Serial.println("Received Wi-Fi credentials");
            Serial.printf("\tSSID : %s\n", (const char *) sys_event->event_info.prov_cred_recv.ssid);
            Serial.printf("\tPassword : %s\n", (const char *) sys_event->event_info.prov_cred_recv.password);
            break;
        case ARDUINO_EVENT_PROV_INIT:
            wifi_prov_mgr_disable_auto_stop(10000);
            break;
        case ARDUINO_EVENT_PROV_CRED_SUCCESS:
            Serial.println("Stopping Provisioning");
            wifi_prov_mgr_stop_provisioning();
            break;
    }
}

void checkManualSwitch() {
    bool current_state = digitalRead(manual_switch_pin);

    // Detect falling edge (HIGH to LOW)
    if (last_manual_switch_state == HIGH && current_state == LOW) {
        relay_state = !relay_state;
        updateRelayState(relay_state);
    }

    last_manual_switch_state = current_state;
}

void setup() {
    Serial.begin(115200);

    // Setup PWM for servo
    ledcSetup(ledChannel, freq, resolution);
    ledcAttachPin(servo_pin, ledChannel);
    setServoAngle(100); // Set initial angle (no press)

    pinMode(relay_pin, OUTPUT);
    pinMode(manual_switch_pin, INPUT_PULLUP);
    digitalWrite(relay_pin, relay_state ? HIGH : LOW);

    Node my_node = RMaker.initNode("Relay Controller");

    my_switch.addCb(write_callback);
    my_node.addDevice(my_switch);

    my_pc.addCb(write_callback);
    my_node.addDevice(my_pc);

    RMaker.enableOTA(OTA_USING_PARAMS);
    RMaker.enableTZService();
    RMaker.enableSchedule();

    Serial.println("Starting ESP-RainMaker");
    RMaker.start();

    WiFi.onEvent(sysProvEvent);
    WiFi.config(INADDR_NONE, INADDR_NONE, INADDR_NONE, IPAddress(8, 8, 8, 8));
    WiFiProv.beginProvision(WIFI_PROV_SCHEME_BLE, WIFI_PROV_SCHEME_HANDLER_FREE_BTDM, WIFI_PROV_SECURITY_1, pop, service_name);
}

void loop() {
    if (digitalRead(gpio_reset) == LOW) {
        delay(100);
        int startTime = millis();
        while (digitalRead(gpio_reset) == LOW) delay(50);
        int endTime = millis();

        if ((endTime - startTime) > 10000) {
            Serial.printf("Reset to factory.\n");
            wifi_connected = 0;
            RMakerFactoryReset(2);
        } else if ((endTime - startTime) > 3000) {
            Serial.printf("Reset Wi-Fi.\n");
            wifi_connected = 0;
            RMakerWiFiReset(2);
        }
    }

    checkManualSwitch();
    delay(100);
}

