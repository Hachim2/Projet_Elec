#include <Arduino.h>

const int pinBPM = A0;

void setup(){
    pinMode(pinBPM, INPUT);
    Serial.begin(9600);
}

void loop(){
    int valeur = analogRead(pinBPM);
    Serial.println("Valeur capteur : ");
    Serial.println(valeur);
    delay(10);
}