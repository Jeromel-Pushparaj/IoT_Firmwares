#include <Arduino.h>

#define relay_pin 21  // GPIO pin D1 (used as an output)
#define SwitchState 18 // GPIO pin D5 (used as an Input)


unsigned long starttimer = 0;  
const unsigned long fiveMin = 8 * 60 * 1000ul;

bool timerActive = false;
bool switchs = false;

void setup() {
  // put your setup code here, to run once:
    pinMode(relay_pin, OUTPUT);  
    pinMode(SwitchState, INPUT);
    digitalWrite(relay_pin, LOW);
    Serial.begin(115200);    

}

void loop() {
  // put your main code here, to run repeatedly:
  int s = digitalRead(SwitchState);

  if(s == HIGH){
    Serial.println("F1");
    digitalWrite(relay_pin, HIGH);
    switchs = true;

  }else if(switchs == true && s == LOW){
    Serial.println("else");
    timerActive = true;             // Start the timer
    starttimer = millis();  
    switchs = false;
  }

  if (timerActive == true) {
    Serial.println("F2");

    if (millis() - starttimer >= fiveMin) {
        Serial.println("F3");
        digitalWrite(relay_pin, LOW);   // Turn off the relay after 5 minutes
        timerActive = false;            // Reset the timer
        Serial.println("Five minutes elapsed. Relay OFF");
    }
  }
  delay(100);
}
