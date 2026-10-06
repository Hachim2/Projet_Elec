#include "bpm.hpp"

void init_bpm() {
    Serial.begin(115200);
    pinMode(PIN_PPG, INPUT);
}

float actualiser_bpm() {
    const unsigned long PERIODE_LECTURE = 5; // ms
    const uint8_t TAILLE_MOYENNE = 100;

    // Seuils appliqués au signal recentré.
    const int SEUIL_HAUT = 200;
    const int SEUIL_BAS = 50;

    // Intervalles acceptés : 500 à 2000 ms, soit 30 à 120 BPM.
    const unsigned long INTERVALLE_MIN = 500;
    const unsigned long INTERVALLE_MAX = 2000;
    const unsigned long DELAI_SANS_BATTEMENT = 2500;

    const uint8_t NB_INTERVALLES = 5;

    // Variables conservées entre les appels.
    static bool initialise = false;
    static unsigned long debut = 0;
    static unsigned long derniereLecture = 0;
    static unsigned long dernierBattement = 0;
    static bool premierBattementRecu = false;
    static bool detectionArmee = false;

    static int valeurs[TAILLE_MOYENNE];
    static long somme = 0;
    static uint8_t indice = 0;

    static unsigned long intervalles[NB_INTERVALLES];
    static uint8_t indiceIntervalle = 0;
    static uint8_t nombreIntervalles = 0;

    static float bpm = 0.0f;

    const unsigned long maintenant = millis();

    // Lire le capteur au maximum une fois toutes les 5 ms.
    if (initialise &&
        maintenant - derniereLecture < PERIODE_LECTURE) {
        return bpm;
    }

    derniereLecture = maintenant;
    const int brut = analogRead(PIN_PPG);

    // Initialiser le tampon avec la première mesure.
    if (!initialise) {
        for (uint8_t i = 0; i < TAILLE_MOYENNE; i++) {
            valeurs[i] = brut;
        }

        somme = (long)brut * TAILLE_MOYENNE;
        debut = maintenant;
        initialise = true;
        return 0.0f;
    }

    // 1. Moyenne glissante des 100 dernières mesures.
    somme -= valeurs[indice];
    valeurs[indice] = brut;
    somme += brut;

    indice = (indice + 1) % TAILLE_MOYENNE;

    const float moyenne = (float)somme / TAILLE_MOYENNE;

    // 2. Recentrage du signal.
    const float signal = brut - moyenne;

    // Affichage Teleplot : une variable par ligne.
    //Serial.print(">Signal:");
    //Serial.println(signal);

    

    // Attendre une seconde avant de détecter les battements.
    if (maintenant - debut < 1000) {
        return 0.0f;
    }

    // Réinitialiser après 2,5 s sans battement retenu.
    if (premierBattementRecu &&
        maintenant - dernierBattement > DELAI_SANS_BATTEMENT) {
        bpm = 0.0f;
        premierBattementRecu = false;
        nombreIntervalles = 0;
        indiceIntervalle = 0;
        detectionArmee = false;
    }

    // 3. Réarmer après la descente sous le seuil bas.
    if (signal <= SEUIL_BAS) {
        detectionArmee = true;
    }

    // 4. Détecter le passage au-dessus du seuil haut.
    if (detectionArmee && signal >= SEUIL_HAUT) {
        detectionArmee = false;

        // Premier battement : mémoriser uniquement son instant.
        if (!premierBattementRecu) {
            dernierBattement = maintenant;
            premierBattementRecu = true;
            return bpm;
        }

        const unsigned long intervalle =
            maintenant - dernierBattement;

        // Ignorer les détections trop proches.
        if (intervalle < INTERVALLE_MIN) {
            return bpm;
        }

        dernierBattement = maintenant;

        // Intervalle trop long : recommencer la moyenne.
        if (intervalle > INTERVALLE_MAX) {
            nombreIntervalles = 0;
            indiceIntervalle = 0;
            bpm = 0.0f;
            return bpm;
        }

        // 5. Conserver les cinq derniers intervalles acceptés.
        intervalles[indiceIntervalle] = intervalle;
        indiceIntervalle =
            (indiceIntervalle + 1) % NB_INTERVALLES;

        if (nombreIntervalles < NB_INTERVALLES) {
            nombreIntervalles++;
        }

        unsigned long sommeIntervalles = 0;

        for (uint8_t i = 0; i < nombreIntervalles; i++) {
            sommeIntervalles += intervalles[i];
        }

        const float intervalleMoyen =
            (float)sommeIntervalles / nombreIntervalles;

        // 6. Calcul du BPM.
        bpm = 60000.0f / intervalleMoyen;

        Serial.print(">BPM:");
        Serial.println(bpm);
    }

    return bpm;
}

void setup() {
    init_bpm();
}

void loop() {
    actualiser_bpm();
}