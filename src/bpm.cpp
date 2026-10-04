#include "bpm.hpp"

static float bpm = 0;

float lire_bpm() {
    return bpm;
}

void init_bpm() {
    Serial.begin(115200);
    pinMode(PIN_PPG, INPUT);
}

void actualiser_bpm() {
    static unsigned long dernierEchantillon = 0;
    static int valeurs[NB_ECHANTILLONS] = {};
    static uint8_t indice = 0, nombre = 0;
    static unsigned long somme = 0;
    static unsigned long dernierBattement = 0;
    static bool premierBattement = true, detectionArmee = false;

    unsigned long maintenant = millis();
    if (maintenant - dernierEchantillon < PERIODE_LECTURE) return;
    dernierEchantillon = maintenant;

    int brut = analogRead(PIN_PPG);
    // Remplace la lecture la plus ancienne par la nouvelle.
    somme -= valeurs[indice];
    valeurs[indice] = brut;
    somme += brut;
    indice = (indice + 1) % NB_ECHANTILLONS;
    if (nombre < NB_ECHANTILLONS) nombre++;

    float moyenne = (float)somme / nombre;
    float signalRecentre = brut - moyenne;

    // Attend une fenetre complete avant de detecter les battements.
    if (nombre < NB_ECHANTILLONS) return;

    if (!premierBattement && maintenant - dernierBattement > INTERVALLE_MAX) {
        premierBattement = true;
        bpm = 0;
    }

    // Un nouveau pic exige d'abord un retour sous la moyenne.
    if (signalRecentre < 0) detectionArmee = true;
    if (detectionArmee && signalRecentre >= SEUIL_BATTEMENT) {
        detectionArmee = false;
        unsigned long intervalle = maintenant - dernierBattement;
        if (premierBattement || intervalle >= INTERVALLE_MIN) {
            if (!premierBattement) bpm = 60000.0f / intervalle;
            dernierBattement = maintenant;
            premierBattement = false;
        }
    }

    Serial.print("Brut : "); Serial.print(brut);
    Serial.print(" | Moyenne : "); Serial.print(moyenne);
    Serial.print(" | Recentre : "); Serial.print(signalRecentre);
    Serial.print(" | BPM : "); Serial.println(bpm);
}
