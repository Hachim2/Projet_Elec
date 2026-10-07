#include <Arduino.h>
#include <EEPROM.h>
#include "../donnees.hpp"

// Test de la sauvegarde EEPROM + RTC, sans ecran ni capteur.
// Moniteur serie a 115200 bauds, puis envoyer :
//   e : ecrire une mesure de test (BPM 72, heure du RTC)
//   l : lister les mesures relues depuis l'EEPROM
//   d : afficher les premiers octets bruts de l'EEPROM
//   c : tout effacer
// Test de persistance : envoyer e, debrancher la carte, la rebrancher,
// envoyer l : la mesure doit toujours etre la.

static void lister() {
    const uint8_t n = nombre_enregistrements();
    Serial.print(F("Mesures en EEPROM : "));
    Serial.println(n);

    char texte[TAILLE_TEXTE_ENREGISTREMENT];
    for (uint8_t i = 0; i < n; ++i) {
        formater_enregistrement(lire_enregistrement(i), texte);
        Serial.print(i);
        Serial.print(F(" : "));
        Serial.println(texte);
    }
}

static void afficher_octets_bruts() {
    // 0 : controle (0xB7), 1 : nombre, puis 5 octets par mesure.
    for (int adresse = 0; adresse < 22; ++adresse) {
        const uint8_t octet = EEPROM.read(adresse);
        if (octet < 0x10) Serial.print('0');
        Serial.print(octet, HEX);
        Serial.print(adresse == 1 ? F(" | ") : F(" "));
    }
    Serial.println();
}

static void ecrire_test() {
    Enregistrement e;
    switch (enregistrer_bpm(72, &e)) {
    case Resultat::Ok: {
        char texte[TAILLE_TEXTE_ENREGISTREMENT];
        formater_enregistrement(e, texte);
        Serial.print(F("OK, ecrit et relu : "));
        Serial.println(texte);
        break;
    }
    case Resultat::HorlogeInvalide:
        Serial.println(F("Echec : horloge invalide (regler avec test_rtc)."));
        break;
    case Resultat::MemoirePleine:
        Serial.println(F("Echec : memoire pleine (envoyer c)."));
        break;
    case Resultat::ErreurEcriture:
        Serial.println(F("Echec : la relecture de l'EEPROM ne correspond pas !"));
        break;
    case Resultat::PasDeBpm:
        break;
    }
}

void setup() {
    Serial.begin(115200);
    init_donnees();
    Serial.println(F("=== Test EEPROM ==="));
    Serial.println(F("e : ecrire, l : lister, d : octets bruts, c : effacer"));
    lister();
}

void loop() {
    if (Serial.available() == 0) return;

    switch (Serial.read()) {
    case 'e': ecrire_test(); break;
    case 'l': lister(); break;
    case 'd': afficher_octets_bruts(); break;
    case 'c':
        effacer_enregistrements();
        Serial.println(F("Tout efface."));
        break;
    }
}
