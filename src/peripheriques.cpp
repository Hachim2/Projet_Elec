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

// Bouton poussoir avec anti-rebond, action au relachement.
struct Bouton {
    uint8_t broche;
    uint8_t niveau_appui; // LOW : rappel interne, HIGH : tirage externe vers GND.
    int derniere_lecture;
    int etat_stable;
    unsigned long dernier_changement;
};

Bouton bouton_retour = {BTN_RETOUR, BTN_RETOUR_APPUI, 0, 0, 0};
Bouton bouton_son = {BTN_SON, BTN_SON_APPUI, 0, 0, 0};

void init_bouton(Bouton& b) {
    pinMode(b.broche, b.niveau_appui == LOW ? INPUT_PULLUP : INPUT);
    b.derniere_lecture = digitalRead(b.broche);
    b.etat_stable = b.derniere_lecture;
    b.dernier_changement = millis();
}

// Renvoie true une fois au relachement (rebonds filtres).
bool bouton_relache(Bouton& b) {
    const int lecture = digitalRead(b.broche);
    const unsigned long maintenant = millis();
    if (lecture != b.derniere_lecture) {
        b.derniere_lecture = lecture;
        b.dernier_changement = maintenant;
    }

    if (lecture == b.etat_stable || maintenant - b.dernier_changement < ANTI_REBOND_MS) {
        return false;
    }

    // Comme l'encodeur : l'action a lieu au relachement.
    b.etat_stable = lecture;
    return b.etat_stable != b.niveau_appui;
}
}

void init_peripheriques() {
    pinMode(BUZZER, OUTPUT);
    digitalWrite(BUZZER, LOW);

    pinMode(LED_ROUGE, OUTPUT);
    pinMode(LED_VERTE, OUTPUT);
    pinMode(LED_JAUNE, OUTPUT);
    afficher_zone_bpm(0);

    init_bouton(bouton_retour);
    init_bouton(bouton_son);
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
    return bouton_relache(bouton_retour);
}

void actualiser_bouton_son() {
    if (!bouton_relache(bouton_son)) return;
    reglages.son = !reglages.son;
    sauver_reglages();
}
