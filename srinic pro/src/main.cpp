//https://github.com/espressif/arduino-esp32/issues/8176 To run this program in esp32 follow this issue in github

#include <AppInsights.h>
#include "RMaker.h"
#include "WiFi.h"
#include "WiFiProv.h"
#include <wifi_provisioning/manager.h>
#include <SimpleTimer.h>

// Set Default Values
#define DEFAULT_RELAY_MODE false

// BLE Credentials
const char *service_name = "PROV_12345";  // Change this to your desired service name
const char *pop = "12345678";             // Change this to your Proof of Possession (PoP)

// GPIO Pin Configuration
static uint8_t gpio_reset = 0;
static uint8_t relay_pin = 21;
static uint8_t acSignal = 26;

static uint8_t indi = 2;
bool relay_state = DEFAULT_RELAY_MODE;

bool wifi_connected = false;

// Declaring Relay as a Standard Switch Device
static Switch my_switch("AC", &acSignal);

bool toggleState = false;
bool switchState = true;

SimpleTimer Timer;
void triger(){
    digitalWrite(relay_pin, HIGH);
    delay(1000);
    digitalWrite(relay_pin, LOW);
}
// Callback Function to Handle State Changes
void write_callback(Device *device, Param *param, const param_val_t val, void *priv_data, write_ctx_t *ctx) {
    const char *device_name = device->getDeviceName();
    const char *param_name = param->getParamName();

    if (strcmp(device_name, "AC") == 0) {
        if (strcmp(param_name, "Power") == 0) {
            relay_state = val.val.b;
            toggleState = val.val.b;
            // (toggleState == false || digitalRead(acSignal)==LOW) ? triger(): digitalWrite(indi, HIGH);
            // (toggleState == true || digitalRead(acSignal) == HIGH) ?  triger():digitalWrite(indi, LOW); 
            // if(toggleState == false || digitalRead(acSignal)==LOW){
            //     triger();
            //     Serial.println("Turn on");
            // }else {
            //     digitalWrite(indi, HIGH);
            //     delay(500);
            //     digitalWrite(indi, LOW);
            //     Serial.println("skip the on");
            // }
            
            if((toggleState == true && digitalRead(acSignal) == HIGH) || (toggleState == false && digitalRead(acSignal)==LOW)){
                Serial.println("skip two way");
            } else{
                Serial.println("Trigering");
                triger(); 
            }
            
            // param->updateAndReport(val);
            
        }
    }
}

// Event Handler for System Events
void sysProvEvent(arduino_event_t *sys_event)
{
    switch (sys_event->event_id) {      
        case ARDUINO_EVENT_PROV_START:
#if CONFIG_IDF_TARGET_ESP32
        Serial.printf("\nProvisioning Started with name \"%s\" and PoP \"%s\" on BLE\n", service_name, pop);
        printQR(service_name, pop, "ble");
#else
        Serial.printf("\nProvisioning Started with name \"%s\" and PoP \"%s\" on SoftAP\n", service_name, pop);
        printQR(service_name, pop, "softap");
#endif        
        break;
        case ARDUINO_EVENT_WIFI_STA_CONNECTED:
        Serial.printf("\nConnected to Wi-Fi!\n");
        // digitalWrite(wifiLed, true);
        break;
    }
}

// void sysProvEvent(arduino_event_t *sys_event) {
//     switch (sys_event->event_id) {
//         case ARDUINO_EVENT_PROV_START:
//             Serial.printf("Provisioning Started with service name \"%s\" and PoP \"%s\"\n", service_name, pop);
//             printQR(service_name, pop, "ble");
//             break;
//         case ARDUINO_EVENT_WIFI_STA_CONNECTED:
//             Serial.println("Connected to Wi-Fi!");
//             wifi_connected = true;
//             break;
//         case ARDUINO_EVENT_PROV_CRED_RECV:
//             Serial.println("Received Wi-Fi credentials");
//             Serial.printf("\tSSID : %s\n", (const char *) sys_event->event_info.prov_cred_recv.ssid);
//             Serial.printf("\tPassword : %s\n", (const char *) sys_event->event_info.prov_cred_recv.password);
//             break;
//         case ARDUINO_EVENT_PROV_INIT:
//             wifi_prov_mgr_disable_auto_stop(10000);
//             break;
//         case ARDUINO_EVENT_PROV_CRED_SUCCESS:
//             Serial.println("Stopping Provisioning");
//             wifi_prov_mgr_stop_provisioning();
//             break;
//     }
// }

void manual_control()
{
  if (digitalRead(acSignal)==HIGH && toggleState == false) {
    toggleState = true;
    switchState = false;
    my_switch.updateAndReportParam(ESP_RMAKER_DEF_POWER_NAME, toggleState);
    Serial.println("Ac is on state");
    }
    if(digitalRead(acSignal) == LOW && toggleState == true){
        toggleState = false;
        switchState = true;
        my_switch.updateAndReportParam(ESP_RMAKER_DEF_POWER_NAME, toggleState);
        Serial.println("Ac is off state"); 
    }

}

void setup() {
    Serial.begin(115200);

    // Configure Relay GPIO
    pinMode(acSignal, INPUT);
    pinMode(relay_pin, OUTPUT);
    pinMode(indi, OUTPUT);
    digitalWrite(relay_pin, relay_state ? HIGH : LOW);
    digitalWrite(acSignal, LOW);

    // Initialize the ESP-RainMaker Node
    Node my_node;
    my_node = RMaker.initNode("Relay Controller");

    // Register the Switch Device and Callback Function
    my_switch.addCb(write_callback);
    my_node.addDevice(my_switch);

    // Timer.setInterval(2000);

    // Enable OTA, Timezone, and Schedule Services (Optional)
    RMaker.enableOTA(OTA_USING_PARAMS);
    RMaker.enableTZService();
    RMaker.enableSchedule();

    // Start the ESP-RainMaker
    Serial.println("Starting ESP-RainMaker");
    RMaker.start();

    // Setup Wi-Fi Provisioning
    WiFi.onEvent(sysProvEvent);
    WiFiProv.beginProvision(WIFI_PROV_SCHEME_BLE, WIFI_PROV_SCHEME_HANDLER_FREE_BTDM, WIFI_PROV_SECURITY_1, pop, service_name);
}


void loop() {
  if (digitalRead(gpio_reset) == LOW) { //Push button pressed
    Serial.printf("Reset Button Pressed!\n");
    // Key debounce handling
    delay(100);
    int startTime = millis();
    while (digitalRead(gpio_reset) == LOW) delay(50);
    int endTime = millis();

    if ((endTime - startTime) > 10000) {
      // If key pressed for more than 10secs, reset all
      Serial.printf("Reset to factory.\n");
      wifi_connected = 0;
      RMakerFactoryReset(2);
    } else if ((endTime - startTime) > 3000) {
      Serial.printf("Reset Wi-Fi.\n");
      wifi_connected = 0;
      // If key pressed for more than 3secs, but less than 10, reset Wi-Fi
      RMakerWiFiReset(2);
    }
  }
    
    delay(100);
    manual_control();
    
}
