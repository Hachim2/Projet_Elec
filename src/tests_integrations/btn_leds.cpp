#include <Arduino.h>

const int LED_ROUGE = 6;
const int LED_VERTE = 5;
const int LED_JAUNE = 4;
const int BOUTON = 2;

int etatLED = 0;  // 0 : éteintes, 1 : rouge, 2 : verte, 3 : jaune
int compteur = 0;

// Variables pour filtrer les rebonds du bouton
int derniereLecture = LOW;
int etatBoutonStable = LOW;
unsigned long dernierChangement = 0;
const unsigned long DELAI_ANTI_REBOND = 30; // millisecondes

void afficherLED() {
  // Tout éteindre avant d'allumer la LED choisie
  digitalWrite(LED_ROUGE, LOW);
  digitalWrite(LED_VERTE, LOW);
  digitalWrite(LED_JAUNE, LOW);

  switch (etatLED) {
    case 1:
      digitalWrite(LED_ROUGE, HIGH);
      break;
    case 2:
      digitalWrite(LED_VERTE, HIGH);
      break;
    case 3:
      digitalWrite(LED_JAUNE, HIGH);
      break;
  }
}

void setup() {
  pinMode(LED_ROUGE, OUTPUT);
  pinMode(LED_VERTE, OUTPUT);
  pinMode(LED_JAUNE, OUTPUT);
  pinMode(BOUTON, INPUT);

  Serial.begin(9600);
  afficherLED();

  Serial.println("Test pret : toutes les LED sont eteintes.");
  Serial.println("Compteur : 0");
}

void loop() {
  int lecture = digitalRead(BOUTON);
  unsigned long maintenant = millis();

  // Chaque changement électrique relance le délai anti-rebond
  if (lecture != derniereLecture) {
    dernierChangement = maintenant;
    derniereLecture = lecture;
  }

  // Accepter un nouvel état seulement s'il est stable depuis 30 ms
  if (maintenant - dernierChangement >= DELAI_ANTI_REBOND) {
    if (lecture != etatBoutonStable) {
      etatBoutonStable = lecture;

      // Réagir au relâchement du bouton
      if (etatBoutonStable == LOW) {
        compteur++;
        etatLED = (etatLED + 1) % 4;

        afficherLED();

        Serial.print("Compteur : ");
        Serial.print(compteur);
        Serial.print(" | LED : ");

        switch (etatLED) {
          case 0: Serial.println("toutes eteintes"); break;
          case 1: Serial.println("rouge"); break;
          case 2: Serial.println("verte"); break;
          case 3: Serial.println("jaune"); break;
        }
      }
    }
  }
}