#include "peripheriques.hpp"
#include "setup.hpp"
#include "donnees.hpp"

namespace {
constexpr unsigned long DUREE_BIP_MS = 40;
constexpr unsigned long ANTI_REBOND_MS = 30;

// Limites des zones de BPM.
constexpr uint8_t BPM_BAS = 60;  // En dessous : jaune.
constexpr uint8_t BPM_HAUT = 100; // Au-dessus : rouge.

// Hauteur du bip selon la zone (Hz) : plus grave quand le coeur est lent.
constexpr unsigned int FREQUENCE_LENT = 500;
constexpr unsigned int FREQUENCE_NORMAL = 1000;
constexpr unsigned int FREQUENCE_RAPIDE = 2000;

int derniere_lecture = !BTN_RETOUR_APPUI;
int etat_stable = !BTN_RETOUR_APPUI;
unsigned long dernier_changement = 0;
}

void init_peripheriques() {
    pinMode(BUZZER, OUTPUT);
    digitalWrite(BUZZER, LOW);

    pinMode(LED_ROUGE, OUTPUT);
    pinMode(LED_VERTE, OUTPUT);
    pinMode(LED_JAUNE, OUTPUT);
    afficher_zone_bpm(0);

    pinMode(BTN_RETOUR, BTN_RETOUR_APPUI == LOW ? INPUT_PULLUP : INPUT);
    derniere_lecture = digitalRead(BTN_RETOUR);
    etat_stable = derniere_lecture;
    dernier_changement = millis();
}

void bip_buzzer(uint8_t bpm) {
    if (!reglages.son) return;

    unsigned int frequence = FREQUENCE_NORMAL; // Aussi tant que bpm vaut 0.
    if (bpm > 0 && bpm < BPM_BAS) frequence = FREQUENCE_LENT;
    if (bpm > BPM_HAUT) frequence = FREQUENCE_RAPIDE;

    // tone() (Timer2) coupe le son tout seul apres DUREE_BIP_MS.
    tone(BUZZER, frequence, DUREE_BIP_MS);
}

void afficher_zone_bpm(uint8_t bpm) {
    const bool mesure = bpm > 0 && reglages.leds;
    digitalWrite(LED_JAUNE, mesure && bpm < BPM_BAS);
    digitalWrite(LED_VERTE, mesure && bpm >= BPM_BAS && bpm <= BPM_HAUT);
    digitalWrite(LED_ROUGE, mesure && bpm > BPM_HAUT);
}

bool lire_bouton_retour() {
    const int lecture = digitalRead(BTN_RETOUR);
    const unsigned long maintenant = millis();
    if (lecture != derniere_lecture) {
        derniere_lecture = lecture;
        dernier_changement = maintenant;
    }

    if (lecture == etat_stable || maintenant - dernier_changement < ANTI_REBOND_MS) {
        return false;
    }

    // Comme l'encodeur : l'action a lieu au relachement.
    etat_stable = lecture;
    return etat_stable != BTN_RETOUR_APPUI;
}
