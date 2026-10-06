#include <Arduino.h>
#include "../bpm.hpp"

// Test du module bpm seul : capteur PPG sur A0, aucun ecran.
// Ouvrir Teleplot (ou le moniteur serie) a 115200 bauds.

constexpr unsigned long PERIODE_AFFICHAGE = 200; // ms

void setup() {
    Serial.begin(115200);
    init_bpm();
    Serial.println("Test BPM : poser le doigt sur le capteur.");
}

void loop() {
    static unsigned long dernierAffichage = 0;
    const float bpm = actualiser_bpm();

    // 5 points par seconde : une courbe lisible sans saturer le port serie.
    const unsigned long maintenant = millis();
    if (maintenant - dernierAffichage >= PERIODE_AFFICHAGE) {
        dernierAffichage = maintenant;
        Serial.print(">BPM:");
        Serial.println(bpm);
    }
}
