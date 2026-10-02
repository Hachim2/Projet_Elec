#include <Arduino.h>

#define PIN_A 2
#define PIN_B 3
#define PIN_BOUTON 4

int ancienA;

int derniereLectureBouton = HIGH;
int etatBoutonStable = HIGH;
uint32_t dernierChangementBouton = 0;

const uint32_t ANTI_REBOND = 30;

void setup() {
    Serial.begin(9600);

    // Rotation : A sur D2, B sur D3 et commun C sur GND.
    // Les contacts ouverts sont lus HIGH, les contacts fermes LOW.
    pinMode(PIN_A, INPUT_PULLUP);
    pinMode(PIN_B, INPUT_PULLUP);
    // Bouton entre D4 et GND : HIGH au repos, LOW a l'appui.
    pinMode(PIN_BOUTON, INPUT_PULLUP);

    ancienA = digitalRead(PIN_A);
    derniereLectureBouton = digitalRead(PIN_BOUTON);
    etatBoutonStable = derniereLectureBouton;
    dernierChangementBouton = millis();
}

void loop() {

    // ===== Rotation encodeur =====
    int etatA = digitalRead(PIN_A);

    if (etatA != ancienA && etatA == HIGH) {

        if (digitalRead(PIN_B) != etatA) {
            Serial.println("Droite");
        } else {
            Serial.println("Gauche");
        }
    }

    ancienA = etatA;

    // ===== Bouton encodeur =====
    const int bouton = digitalRead(PIN_BOUTON);
    const uint32_t maintenant = millis();

    if (bouton != derniereLectureBouton) {
        dernierChangementBouton = maintenant;
        derniereLectureBouton = bouton;
    }

    // Valider le nouvel etat seulement apres 30 ms sans changement.
    if (maintenant - dernierChangementBouton >= ANTI_REBOND &&
        bouton != etatBoutonStable) {
        etatBoutonStable = bouton;
        if (etatBoutonStable == LOW) {
            Serial.println("Appui");
        }
    }
}
