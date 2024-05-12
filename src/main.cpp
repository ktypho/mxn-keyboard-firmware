#define DEBUG

#include <Arduino.h>

#include <Keyboard.h>
#include <Mouse.h>

void setup() {
#ifdef DEBUG
  Serial.begin(9600);
#endif
}

void loop() {
  
}