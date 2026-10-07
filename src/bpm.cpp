#include "bpm.hpp"
#include "setup.hpp"
#include <avr/interrupt.h>
#include <util/atomic.h>

// Decommenter pour suivre le BPM dans Teleplot (moniteur serie a 115200).
// #define BPM_TELEPLOT

namespace {
// --- Echantillonnage (interruption Timer1) ---
constexpr uint8_t PERIODE_ECHANTILLON_MS = 5; // 200 mesures par seconde.
constexpr uint8_t TAILLE_MOYENNE = 100;       // Ligne de base sur 0,5 s.
constexpr uint16_t STABILISATION_MS = 1000;   // Attente avant la detection.

// Seuils appliques au signal recentre (unites ADC), avec hysteresis.
constexpr int SEUIL_HAUT = 200;
constexpr int SEUIL_BAS = 50;

// Intervalles acceptes : 330 a 2000 ms, soit environ 180 a 30 BPM.
constexpr uint16_t INTERVALLE_MIN = 330;
constexpr uint16_t INTERVALLE_MAX = 2000;
constexpr uint16_t DELAI_SANS_BATTEMENT = 2500;

// --- Moyenne glissante du BPM ---
constexpr uint8_t NB_INTERVALLES = 10;
constexpr uint8_t NB_MIN_POUR_FILTRE = 3;  // Avant, tout intervalle valide compte.
constexpr uint8_t ECART_MAX_POURCENT = 30; // Rejet si trop loin de la moyenne.
constexpr uint8_t REJETS_AVANT_RESET = 3;  // Le rythme a vraiment change.

// Partage entre l'interruption et la boucle principale.
volatile uint32_t temps_ms = 0; // Horloge du capteur : +5 ms par mesure.
volatile uint32_t dernier_battement_ms = 0;
volatile uint16_t intervalle_detecte = 0;
volatile bool nouveau_battement = false;
volatile bool battement_a_signaler = false; // Pour le bip du buzzer.

// File des echantillons du signal pour le trace (32 x 5 ms = 160 ms de
// marge pendant les dessins d'ecran).
constexpr uint8_t TAILLE_FILE_SIGNAL = 32; // Puissance de 2.
volatile int8_t file_signal[TAILLE_FILE_SIGNAL];
volatile uint8_t file_ecriture = 0; // Avance dans l'interruption.
uint8_t file_lecture = 0;           // Avance dans la boucle principale.

// Utilise uniquement dans l'interruption.
int valeurs[TAILLE_MOYENNE];
int32_t somme = 0;
uint8_t indice = 0;
bool detection_armee = false;
bool battement_precedent = false;

// Utilise uniquement dans la boucle principale.
uint16_t intervalles[NB_INTERVALLES];
uint8_t indice_intervalle = 0;
uint8_t nombre_intervalles = 0;
uint32_t somme_intervalles = 0;
uint8_t rejets_consecutifs = 0;
float bpm = 0.0f;

void reinitialiser_moyenne() {
    indice_intervalle = 0;
    nombre_intervalles = 0;
    somme_intervalles = 0;
    rejets_consecutifs = 0;
    bpm = 0.0f;
}

void ajouter_intervalle(uint16_t intervalle) {
    if (nombre_intervalles == NB_INTERVALLES) {
        somme_intervalles -= intervalles[indice_intervalle];
    } else {
        ++nombre_intervalles;
    }
    intervalles[indice_intervalle] = intervalle;
    somme_intervalles += intervalle;
    indice_intervalle = (indice_intervalle + 1) % NB_INTERVALLES;

    // BPM = 60000 / intervalle moyen.
    bpm = 60000.0f * nombre_intervalles / somme_intervalles;
}
}

// Mesure + detection a frequence fixe, independamment du temps passe a
// dessiner les ecrans. Que des calculs entiers : environ 150 us par appel.
ISR(TIMER1_COMPA_vect) {
    const int brut = analogRead(PIN_PPG);
    temps_ms += PERIODE_ECHANTILLON_MS;

    // 1. Moyenne glissante des 100 dernieres mesures (ligne de base).
    somme += brut - valeurs[indice];
    valeurs[indice] = brut;
    if (++indice >= TAILLE_MOYENNE) indice = 0;

    // 2. Recentrage du signal.
    const int signal = brut - static_cast<int>(somme / TAILLE_MOYENNE);

    // Echantillon pour le trace facon moniteur d'hopital.
    file_signal[file_ecriture % TAILLE_FILE_SIGNAL] =
        constrain(signal / DIVISEUR_SIGNAL_TRACE, -127, 127);
    ++file_ecriture;

    if (temps_ms < STABILISATION_MS) return;

    // 3. Rearmer apres la descente sous le seuil bas.
    if (signal <= SEUIL_BAS) {
        detection_armee = true;
        return;
    }

    // 4. Battement = passage au-dessus du seuil haut.
    if (!detection_armee || signal < SEUIL_HAUT) return;
    detection_armee = false;

    const uint32_t ecart = temps_ms - dernier_battement_ms;
    if (battement_precedent) {
        // Trop proche : rebond ou onde dicrote, on l'ignore.
        if (ecart < INTERVALLE_MIN) return;
        intervalle_detecte = ecart > 0xFFFF ? 0xFFFF : ecart;
        nouveau_battement = true;
    }
    dernier_battement_ms = temps_ms;
    battement_precedent = true;
    battement_a_signaler = true;
}

void init_bpm() {
#ifdef BPM_TELEPLOT
    Serial.begin(115200);
#endif
    pinMode(PIN_PPG, INPUT);

    // Remplir la ligne de base avec la premiere mesure.
    const int premiere = analogRead(PIN_PPG);
    for (uint8_t i = 0; i < TAILLE_MOYENNE; i++) {
        valeurs[i] = premiere;
    }
    somme = static_cast<int32_t>(premiere) * TAILLE_MOYENNE;

    // Timer1 en mode CTC, prescaler 64 : 16 MHz / 64 = 250 kHz,
    // soit 1250 ticks pour 5 ms. Les PWM des broches D9/D10 sont perdues.
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
        TCCR1A = 0;
        TCCR1B = _BV(WGM12) | _BV(CS11) | _BV(CS10);
        TCNT1 = 0;
        OCR1A = F_CPU / 64 / 1000 * PERIODE_ECHANTILLON_MS - 1;
        TIFR1 = _BV(OCF1A);
        TIMSK1 = _BV(OCIE1A);
    }
}

float actualiser_bpm() {
    bool nouveau;
    uint16_t intervalle;
    uint32_t silence;
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
        nouveau = nouveau_battement;
        nouveau_battement = false;
        intervalle = intervalle_detecte;
        silence = temps_ms - dernier_battement_ms;
    }

    // Doigt retire : plus de battement depuis 2,5 s.
    if (nombre_intervalles > 0 && silence > DELAI_SANS_BATTEMENT) {
        reinitialiser_moyenne();
    }
    if (!nouveau) return bpm;

    // Intervalle trop long : on recommence la moyenne.
    if (intervalle > INTERVALLE_MAX) {
        reinitialiser_moyenne();
        return bpm;
    }

    // 5. Rejeter les valeurs aberrantes (battement rate = intervalle double,
    // faux battement = intervalle coupe en deux). Apres plusieurs rejets
    // d'affilee, c'est le rythme qui a change : on repart de zero.
    if (nombre_intervalles >= NB_MIN_POUR_FILTRE) {
        const uint16_t moyenne = somme_intervalles / nombre_intervalles;
        const uint16_t ecart = intervalle > moyenne ? intervalle - moyenne
                                                    : moyenne - intervalle;
        if (static_cast<uint32_t>(ecart) * 100 >
            static_cast<uint32_t>(moyenne) * ECART_MAX_POURCENT) {
            if (++rejets_consecutifs < REJETS_AVANT_RESET) return bpm;
            reinitialiser_moyenne();
        }
    }
    rejets_consecutifs = 0;

    // 6. Moyenne glissante sur les 10 derniers intervalles.
    ajouter_intervalle(intervalle);

#ifdef BPM_TELEPLOT
    Serial.print(">BPM:");
    Serial.println(bpm);
#endif
    return bpm;
}

bool lire_echantillon_signal(int8_t* valeur) {
    uint8_t ecriture;
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
        ecriture = file_ecriture;
    }
    if (file_lecture == ecriture) return false;

    // Boucle trop lente : on saute les echantillons deja ecrases.
    if (static_cast<uint8_t>(ecriture - file_lecture) > TAILLE_FILE_SIGNAL) {
        file_lecture = ecriture - TAILLE_FILE_SIGNAL;
    }
    *valeur = file_signal[file_lecture % TAILLE_FILE_SIGNAL];
    ++file_lecture;
    return true;
}

bool battement_detecte() {
    bool battement;
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
        battement = battement_a_signaler;
        battement_a_signaler = false;
    }
    return battement;
}

float lire_bpm() {
    return bpm;
}
