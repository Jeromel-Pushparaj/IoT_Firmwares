#include <WiFi.h>
#include <ESPAsyncWebServer.h>

#define LED_PIN 5   // ESP32 pin GPIO18 connected to LED

const char *ssid = "Light";     // CHANGE IT
const char *password = "12354768";       // CHANGE IT

AsyncWebServer server(80);

int LED_state = LOW;

String getHTML() {
  String html = "<!DOCTYPE HTML>";
  html += "<html>";
  html += "<head>";
  html += "<link rel='icon' href='data:,'>";
  html += "</head>";
  html += "<p>LED state: <span style='color: red;'>";

  if (LED_state == LOW)
    html += "OFF";
  else
    html += "ON";

  html += "</span></p>";
  html += "<a href='/led1/on'>Turn ON</a>";
  html += "<br><br>";
  html += "<a href='/led1/off'>Turn OFF</a>";
  html += "</html>";

  return html;
}

// Function to add CORS headers
void addCORSHeaders(AsyncWebServerResponse *response) {
  response->addHeader("Access-Control-Allow-Origin", "*"); // Allow all origins
  response->addHeader("Access-Control-Allow-Methods", "GET, POST, OPTIONS");
  response->addHeader("Access-Control-Allow-Headers", "Content-Type");
}

void setup() {
  Serial.begin(921600);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LED_state);

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

  // Print the ESP32's IP address
  Serial.print("ESP32 Web Server's IP address: ");
  Serial.println(WiFi.localIP());

  // Home page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    Serial.println("ESP32 Web Server: New request received:");
    Serial.println("GET /");
    AsyncWebServerResponse *response = request->beginResponse(200, "text/html", getHTML());
    addCORSHeaders(response); // Add CORS headers
    request->send(response);
  });

  // Route to control the LED
  server.on("/led1/on", HTTP_GET, [](AsyncWebServerRequest *request) {
    Serial.println("ESP32 Web Server: New request received:");
    Serial.println("GET /led1/on");
    LED_state = HIGH;
    digitalWrite(LED_PIN, LED_state);
    AsyncWebServerResponse *response = request->beginResponse(200, "text/html", getHTML());
    addCORSHeaders(response); // Add CORS headers
    request->send(response);
  });

  server.on("/led1/off", HTTP_GET, [](AsyncWebServerRequest *request) {
    Serial.println("ESP32 Web Server: New request received:");
    Serial.println("GET /led1/off");
    LED_state = LOW;
    digitalWrite(LED_PIN, LED_state);
    AsyncWebServerResponse *response = request->beginResponse(200, "text/html", getHTML());
    addCORSHeaders(response); // Add CORS headers
    request->send(response);
  });

  // Status endpoint to confirm device is online
  server.on("/status", HTTP_GET, [](AsyncWebServerRequest *request) {
    Serial.println("ESP32 Web Server: Status check received");
    AsyncWebServerResponse *response = request->beginResponse(200, "text/plain", "Device is online");
    addCORSHeaders(response); // Add CORS headers
    request->send(response);
  });

  // Start the server
  server.begin();
}

void loop() {
  // Your code here
}
