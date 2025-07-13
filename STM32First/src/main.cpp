#include <Arduino.h>

void setup() {
  // put your setup code here, to run once:
  pinMode(13, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  digitalWrite(13, HIGH);
  for(int i=0; i < 1000 * 1000; i++){
    __asm("NOP");
  }
  digitalWrite(13, LOW);
  for(int i=0; i<1000 * 1000; i++){
    __asm("NOP");
  }
}

// put function definitions here:
void trafficLight(){
  
}