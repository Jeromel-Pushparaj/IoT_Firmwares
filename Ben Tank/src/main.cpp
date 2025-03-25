#include <Arduino.h>

#define relay_pin 21  // GPIO pin D1 (used as an output)
#define primarytank 32 // GPIO pin D5 (used as an Input)
#define secondarytank 15  // GPIO pin D1 (used as an input)

bool motorState = false;



void setup() {
  // put your setup code here, to run once:
    pinMode(relay_pin, OUTPUT);  
    pinMode(primarytank, INPUT);
    pinMode(secondarytank, INPUT); 
    digitalWrite(relay_pin, LOW);
    // digitalWrite(primarytank, LOW);
    // digitalWrite(secondarytank, LOW);
    Serial.begin(115200);    

}

void loop() {
  // put your main code here, to run repeatedly:
  int primaryState = digitalRead(primarytank);
  int secondaryState = digitalRead(secondarytank);

  if(primaryState == HIGH && secondaryState == HIGH && motorState == false){
    Serial.println("Motor turned on");
    digitalWrite(relay_pin, HIGH);
    motorState = true;

  }else if((primaryState == LOW || secondaryState == LOW) && motorState == true){
      Serial.println("Motor turned off");
      digitalWrite(relay_pin, LOW);
      motorState = false;
  }
  delay(100);
}
