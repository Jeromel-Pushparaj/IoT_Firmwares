#include <esp32-hal-gpio.h>
#include <HardwareSerial.h>
#include <ESPAsyncWebServer.h>
#include <WiFiMulti.h>

WiFiMulti wifimulti;

#define WIFI_SSID "unkown_network!"
#define WIFI_PASSWORD "Bsnl@1234"

AsyncWebServer server(80);

// GPIO for inbuilt LED on ESP32
// const int LED_PIN = 2;
// Include necessary libraries
// For IR control
//#include <IRremoteESP8266.h>
//#include <IRsend.h>

// Define pins
// const int inputPin = 15;  // GPIO pin connected to the 3V input signal
const int controlPin = 5;   // GPIO pin to control the relay or IR LED

// For IR control
//const uint16_t kIrLed = controlPin;
//IRsend irsend(kIrLed);

bool acState = false; // false: OFF, true: ON

void setup() {
  Serial.begin(921600);
  
  pinMode(controlPin, OUTPUT);
  
  digitalWrite(controlPin, LOW);

    // pinMode(LED_BUILTIN, OUTPUT);
  wifimulti.addAP(WIFI_SSID, WIFI_PASSWORD);

  while (wifimulti.run() != WL_CONNECTED)
  {
    delay(100);
  }
  Serial.println("connected");

  // Print the IP address
  Serial.println(WiFi.localIP());

  // Route for the root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(200, "text/html", 
      "<html>\
        <body>\
          <h1>ESP32 Web Server</h1>\
          <p>Light - <a href=\"/lightOn\"><button>ON</button></a> <a href=\"/lightOff\"><button>OFF</button></a></p>\
          <p>Fan - <a href=\"/fanOn\"><button>ON</button></a> <a href=\"/fanOff\"><button>OFF</button></a></p>\
        </body>\
      </html>");
  });

  // Route to turn the light on
  server.on("/lightOn", HTTP_GET, [](AsyncWebServerRequest *request){
    acState = true;
    digitalWrite(controlPin, HIGH);
    request->send(200, "text/html", 
      "<html>\
        <body>\
          <h1>Light is ON</h1>\
          <a href=\"/\">Go back</a>\
        </body>\
      </html>");
  });

  // Route to turn the light off
  server.on("/lightOff", HTTP_GET, [](AsyncWebServerRequest *request){
    acState = false;
    digitalWrite(controlPin, LOW);
    request->send(200, "text/html", 
      "<html>\
        <body>\
          <h1>Light is OFF</h1>\
          <a href=\"/\">Go back</a>\
        </body>\
      </html>");
  });


  // Start server
  server.begin();
}

void loop() {

}

void controlAC(bool turnOn) {
  // For Relay Control
  if (turnOn) {
    digitalWrite(controlPin, HIGH); // Turn relay ON
  } else {
    digitalWrite(controlPin, LOW);  // Turn relay OFF
  }
/* 
  // For IR Control
  if (turnOn) {
    // Send IR signal to turn AC ON
    irsend.sendNEC(AC_ON_COMMAND, 32);
  } else {
    // Send IR signal to turn AC OFF
    irsend.sendNEC(AC_OFF_COMMAND, 32);
  }
*/
  
  // Add any necessary delays or additional commands as per the AC's requirements
}

