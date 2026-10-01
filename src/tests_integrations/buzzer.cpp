#include <Arduino.h>
#define BUZZER 8

void setup() {
pinMode(BUZZER, OUTPUT);
}

void loop() {
// Buzzer ON pendant 1 seconde
digitalWrite(BUZZER, HIGH);
delay(1000);

// Buzzer OFF pendant 2 secondes
digitalWrite(BUZZER, LOW);
delay(2000);
}