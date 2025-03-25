/*********
  Rui Santos
  Complete project details at https://RandomNerdTutorials.com/esp8266-relay-module-ac-web-server/
  
  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.
*********/

// Import required libraries
#include "ESP8266WiFi.h"
#include "ESPAsyncWebServer.h"

// Set to true to define Relay as Normally Open (NO)
#define RELAY_NO    true

// Assign GPIO pin to the relay
const int relayGPIO = 5;  // Replace with the GPIO pin connected to the relay
const int switch_btn = 14;

// Track the last state of the manual switch
bool lastSwitchState = HIGH;  // Manual switch starts unpressed
bool relayStates = false;      // Initial relay state

// Replace with your network credentials
const char* ssid = "unkown_network!";
const char* password = "Bsnl@1234";

const char* PARAM_INPUT_1 = "state";

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

// HTML code for the web interface
const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html>
<head>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <style>
    html {font-family: Arial; display: inline-block; text-align: center;}
    h2 {font-size: 3.0rem;}
    p {font-size: 3.0rem;}
    body {max-width: 600px; margin:0px auto; padding-bottom: 25px;}
    .switch {position: relative; display: inline-block; width: 120px; height: 68px} 
    .switch input {display: none}
    .slider {position: absolute; top: 0; left: 0; right: 0; bottom: 0; background-color: #ccc; border-radius: 34px}
    .slider:before {position: absolute; content: ""; height: 52px; width: 52px; left: 8px; bottom: 8px; background-color: #fff; -webkit-transition: .4s; transition: .4s; border-radius: 68px}
    input:checked+.slider {background-color: #2196F3}
    input:checked+.slider:before {-webkit-transform: translateX(52px); -ms-transform: translateX(52px); transform: translateX(52px)}
  </style>
</head>
<body>
  <h2>ESP Web Server</h2>
  <h4>Relay Control</h4>
  <label class="switch">
    <input type="checkbox" onchange="toggleCheckbox(this)" id="relay" %RELAY_STATE%>
    <span class="slider"></span>
  </label>
<script>
function toggleCheckbox(element) {
  var xhr = new XMLHttpRequest();
  if(element.checked){ 
    xhr.open("GET", "/update?state=1", true); 
  } else { 
    xhr.open("GET", "/update?state=0", true); 
  }
  xhr.send();
}

// Function to update the switch state in real-time
function updateSwitchState() {
  var xhr = new XMLHttpRequest();
  xhr.open("GET", "/state", true);
  xhr.onload = function() {
    if (xhr.status == 200) {
      var state = xhr.responseText;
      document.getElementById("relay").checked = (state === "1");
    }
  };
  xhr.send();
}

// Periodically update the switch state every second
setInterval(updateSwitchState, 1000);
</script>
</body>
</html>
)rawliteral";

// Function to get the relay state as "1" (on) or "0" (off)
String getRelayState() {
    return digitalRead(relayGPIO) ? "1" : "0";
}

// Function to get the current relay state as a string
String relayState() {
  if (RELAY_NO) {
    return digitalRead(relayGPIO) ? "" : "checked";
  } else {
    return digitalRead(relayGPIO) ? "checked" : "";
  }
}

// Function to handle placeholder replacement
String processor(const String& var) {
  if (var == "RELAY_STATE") {
    return relayState();
  }
  return String();
}

void setup() {
  // Serial port for debugging
  Serial.begin(115200);

  // Configure the relay pin
  pinMode(relayGPIO, OUTPUT);
  pinMode(switch_btn, INPUT_PULLUP);
  digitalWrite(relayGPIO, LOW);


  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }

  // Print ESP8266 Local IP Address
  Serial.println(WiFi.localIP());

  // Route for the web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", index_html, processor);
  });

  // Route to update relay state
  server.on("/update", HTTP_GET, [](AsyncWebServerRequest *request) {
    String state = request->getParam(PARAM_INPUT_1)->value();
    if (state == "1") {
      digitalWrite(relayGPIO, HIGH);
    } else {
      digitalWrite(relayGPIO, LOW);
    }
    request->send(200, "text/plain", "OK");
  });


    // Route to fetch the current relay state
  server.on("/state", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(200, "text/plain", getRelayState());
  });
  // Start the server
  server.begin();
}

void loop() {
  // Read the manual switch state
  bool currentSwitchState = digitalRead(switch_btn);

  // If the manual switch state changes (button press detected)
  if (currentSwitchState != lastSwitchState) {
    if (currentSwitchState == LOW) {  // Button pressed
      relayStates = !relayStates;  // Toggle the relay state
      digitalWrite(relayGPIO, relayStates ? HIGH : LOW);
      Serial.println("Relay toggled by manual switch");
    }
    delay(50);  // Debounce delay
  }

  lastSwitchState = currentSwitchState;  // Update the last switch state  

  delay(500);
}
