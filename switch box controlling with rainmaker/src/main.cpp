/*
  This example demonstrates the ESP RainMaker with a Relay.
*/

#include "RMaker.h"
#include "WiFi.h"
#include "WiFiProv.h"
#include <wifi_provisioning/manager.h>

// Set Default Values
#define DEFAULT_RELAY_MODE true

// BLE Credentials
const char *service_name = "PROV_12345";
const char *pop = "1234567";

// GPIO
static uint8_t gpio_reset = 0;
static uint8_t relay = 21;
bool relay_state = true;

bool wifi_connected = 0;

// Declare Device
static Switch my_switch("Relay", &relay);

void sysProvEvent(arduino_event_t *sys_event)
{
  switch (sys_event->event_id) {
    case ARDUINO_EVENT_PROV_START:
      Serial.printf("\nProvisioning Started with name \"%s\" and PoP \"%s\" on BLE\n", service_name, pop);
      printQR(service_name, pop, "ble");
      break;
    case ARDUINO_EVENT_WIFI_STA_CONNECTED:
      Serial.printf("\nConnected to Wi-Fi!\n");
      wifi_connected = 1;
      delay(500);
      break;
    case ARDUINO_EVENT_PROV_CRED_RECV:
      Serial.println("\nReceived Wi-Fi credentials");
      break;
    case ARDUINO_EVENT_PROV_INIT:
      wifi_prov_mgr_disable_auto_stop(10000);
      break;
    case ARDUINO_EVENT_PROV_CRED_SUCCESS:
      Serial.println("Stopping Provisioning!!!");
      wifi_prov_mgr_stop_provisioning();
      break;
  }
}

void write_callback(Device *device, Param *param, const param_val_t val, void *priv_data, write_ctx_t *ctx)
{
  const char *device_name = device->getDeviceName();
  const char *param_name = param->getParamName();

  if (strcmp(device_name, "Relay") == 0 && strcmp(param_name, "Power") == 0)
  {
    relay_state = val.val.b;
    digitalWrite(relay, relay_state ? HIGH : LOW);
    param->updateAndReport(val);
  }
}

void setup()
{
  Serial.begin(115200);

  pinMode(gpio_reset, INPUT);
  pinMode(relay, OUTPUT);
  digitalWrite(relay, DEFAULT_RELAY_MODE);

  Node my_node = RMaker.initNode("Techiesms");
  my_switch.addCb(write_callback);
  my_node.addDevice(my_switch);

  RMaker.enableOTA(OTA_USING_PARAMS);
  RMaker.enableTZService();
  RMaker.enableSchedule();

  Serial.printf("\nStarting ESP-RainMaker\n");
  RMaker.start();

  WiFi.onEvent(sysProvEvent);

  WiFiProv.beginProvision(WIFI_PROV_SCHEME_BLE, WIFI_PROV_SCHEME_HANDLER_FREE_BTDM, WIFI_PROV_SECURITY_1, pop, service_name);
}

void loop()
{
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
}
