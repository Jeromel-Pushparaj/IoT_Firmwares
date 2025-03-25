#include <esp32-hal-gpio.h>
#include <HardwareSerial.h>
#include <ESPAsyncWebServer.h>
#include <WiFiMulti.h>
#define BLYNK_PRINT Serial
#define BLYNK_TEMPLATE_ID "TMPL3krsHa9Gg"
#define BLYNK_TEMPLATE_NAME "AC"
#include <BlynksimpleEsp32.h>
WiFiMulti wifimulti;

#define WIFI_SSID "Anton H"
#define WIFI_PASSWORD "88888888"

AsyncWebServer server(80); 
char auth[] = "mCbRlJmBeD0Tteo3YLnvcaKlWMfgu7XU";

// Define pins
const int inputPin = 15;    // GPIO pin connected to the 3V input signal
const int outputPin = 5;
// const int controlPin = 2;   // GPIO pin to control the relay or IR LED



//https://blr1.blynk.cloud/external/api/update?token=mCbRlJmBeD0Tteo3YLnvcaKlWMfgu7XU&v0=1

// bool acState = false; // false: OFF, true: ON

// void controlAC(bool turnOn) {
//   int inputState = digitalRead(inputPin);

//   // Update AC state based on button press or 3V input
//   acState = turnOn || (inputState == HIGH);

//   // Control the relay or IR LED based on AC state
//   if (acState) {
//     digitalWrite(controlPin, HIGH);
//     Serial.println("AC Turned ON");
//   } else {
//     digitalWrite(controlPin, LOW);
//     Serial.println("AC Turned OFF");
//   }

//   // Update Blynk app LED (V1) to reflect AC state
//   Blynk.virtualWrite(V0, acState);
// }

// BLYNK_WRITE(V0) // This is the virtual pin where the button in Blynk app is connected
// {
//   bool newACState = param.asInt(); // Get the button state (0 or 1)
//   controlAC(newACState);
// }
// void controlAC(bool turnOn) {
//   int inputState = digitalRead(inputPin);

//   // For Relay Control

// /* 
  // For IR Control
//   if (turnOn) {
//     // Send IR signal to turn AC ON
//     irsend.sendNEC(AC_ON_COMMAND, 32);
//   } else {
//     // Send IR signal to turn AC OFF
//     irsend.sendNEC(AC_OFF_COMMAND, 32);
//   }
// */
  
//   // Add any necessary delays or additional commands as per the AC's requirements
// }


bool buttonState = false;

BlynkTimer timer;

void checkPin() {
  int sensorValue = digitalRead(inputPin);
  if (sensorValue == LOW) {
    if (!buttonState) {
      buttonState = true;
      Blynk.virtualWrite(V0, HIGH); // Update the button on Blynk app
    }
  } else {
    if (buttonState) {
      buttonState = false;
      Blynk.virtualWrite(V0, LOW); // Update the button on Blynk app
    }
  }
}

bool acState = false;

void trigger(){
  digitalWrite(outputPin, HIGH); // Turn on the output pin
  delay(500);                    // Wait for 0.5 seconds
  digitalWrite(outputPin, LOW);  // Turn off the output pin
}
void triggerOutputPin() {
  int sensorValue = digitalRead(inputPin);
  if(sensorValue == HIGH && !acState){
    trigger();
  Serial.println("Ac ON triger sent success");
  acState = true;
  }else{
    digitalWrite(outputPin, HIGH);
    delay(500);
    digitalWrite(outputPin, LOW);
    Serial.println("AC of triger sent success");
    acState = false;
  }
}

// Blynk callback function to handle the button press on V0
BLYNK_WRITE(V0) {
  int buttonState = param.asInt(); // Get the button state

  if (buttonState == 1) { // If the button is pressed
    triggerOutputPin();    // Trigger the output pin
  }else{
    triggerOutputPin();
  }
}
void setup() {
  Serial.begin(921600);
  Blynk.begin(auth, WIFI_SSID, WIFI_PASSWORD, "blynk.cloud", 80);
  
  pinMode(inputPin, INPUT);
  pinMode(outputPin, OUTPUT);
  
  // digitalWrite(controlPin, LOW);

  wifimulti.addAP(WIFI_SSID, WIFI_PASSWORD);

  while (wifimulti.run() != WL_CONNECTED) {
    delay(100);
  }
  Serial.println("connected");
  timer.setInterval(500L, checkPin);
}

void loop() {
  digitalWrite(outputPin, LOW);
  Blynk.run();
  timer.run();

}
