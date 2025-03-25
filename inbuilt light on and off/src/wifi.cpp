#include <ESPAsyncWebServer.h>
#include <WiFiMulti.h>

WiFiMulti wifimulti;

#define WIFI_SSID "unkown_network!"
#define WIFI_PASSWORD "Bsnl@1234"


AsyncWebServer server(80);

// GPIO for inbuilt LED on ESP32
const int LED_PIN = 2;

bool lightState = false;
bool fanState = false;

void setup() {
  // Serial port for debugging purposes
  Serial.begin(921600);

  // Initialize the LED pin as an output
  pinMode(LED_PIN, OUTPUT);

  Serial.begin(921600);
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
    lightState = true;
    digitalWrite(LED_PIN, HIGH);
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
    lightState = false;
    digitalWrite(LED_PIN, LOW);
    request->send(200, "text/html", 
      "<html>\
        <body>\
          <h1>Light is OFF</h1>\
          <a href=\"/\">Go back</a>\
        </body>\
      </html>");
  });

  // Route to turn the fan on
  server.on("/fanOn", HTTP_GET, [](AsyncWebServerRequest *request){
    fanState = true;
    digitalWrite(LED_PIN, HIGH);
    request->send(200, "text/html", 
      "<html>\
        <body>\
          <h1>Fan is ON</h1>\
          <a href=\"/\">Go back</a>\
        </body>\
      </html>");
  });

  // Route to turn the fan off
  server.on("/fanOff", HTTP_GET, [](AsyncWebServerRequest *request){
    fanState = false;
    digitalWrite(LED_PIN, LOW);
    request->send(200, "text/html", 
      "<html>\
        <body>\
          <h1>Fan is OFF</h1>\
          <a href=\"/\">Go back</a>\
        </body>\
      </html>");
  });

  // Start server
  server.begin();
}

void loop() {
  // Nothing needed here
}
