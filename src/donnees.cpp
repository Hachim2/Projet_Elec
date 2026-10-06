#include "donnees.hpp"
#include "setup.hpp"
#include <EEPROM.h>
#include <ThreeWire.h>
#include <RtcDS1302.h>

namespace {
// Organisation de l'EEPROM :
//   0 : octet de controle (EEPROM neuve = 0xFF partout)
//   1 : nombre de mesures
//   2 : mesures, 5 octets chacune (bpm puis secondes)
constexpr int ADRESSE_CONTROLE = 0;
constexpr int ADRESSE_NOMBRE = 1;
constexpr int ADRESSE_MESURES = 2;
constexpr uint8_t TAILLE_MESURE = 5;
constexpr uint8_t VALEUR_CONTROLE = 0xB7;
static_assert(ADRESSE_MESURES + NB_MAX_ENREGISTREMENTS * TAILLE_MESURE <= 1024,
              "L'ATmega328P n'a que 1 Ko d'EEPROM.");

// Ordre : DAT, CLK, CE (comme dans rtc_test.cpp).
ThreeWire liaison(RTC_DAT, RTC_CLK, RTC_CE);
RtcDS1302<ThreeWire> rtc(liaison);

uint8_t nombre = 0;

int adresse(uint8_t i) {
    return ADRESSE_MESURES + i * TAILLE_MESURE;
}

void ecrire_deux_chiffres(char* texte, uint8_t valeur) {
    texte[0] = '0' + valeur / 10;
    texte[1] = '0' + valeur % 10;
}
}

void init_donnees() {
    // L'heure se regle une fois avec rtc_test.cpp ; ici on la lit seulement.
    rtc.Begin();

    if (EEPROM.read(ADRESSE_CONTROLE) != VALEUR_CONTROLE) {
        effacer_enregistrements();
    }
    nombre = min(EEPROM.read(ADRESSE_NOMBRE), NB_MAX_ENREGISTREMENTS);
}

Resultat enregistrer_bpm(uint8_t bpm, Enregistrement* sauve) {
    if (bpm == 0) return Resultat::PasDeBpm;
    if (nombre >= NB_MAX_ENREGISTREMENTS) return Resultat::MemoirePleine;
    if (!rtc.GetIsRunning() || !rtc.IsDateTimeValid()) {
        return Resultat::HorlogeInvalide;
    }

    const Enregistrement e = {bpm, rtc.GetDateTime().TotalSeconds()};
    EEPROM.update(adresse(nombre), e.bpm);
    EEPROM.put(adresse(nombre) + 1, e.secondes);
    // Le compteur est ecrit en dernier : une coupure pendant l'ecriture ne
    // laisse pas de mesure a moitie ecrite dans la liste.
    ++nombre;
    EEPROM.update(ADRESSE_NOMBRE, nombre);

    if (sauve != nullptr) *sauve = e;
    return Resultat::Ok;
}

uint8_t nombre_enregistrements() {
    return nombre;
}

Enregistrement lire_enregistrement(uint8_t i) {
    Enregistrement e = {0, 0};
    if (i >= nombre) return e;
    e.bpm = EEPROM.read(adresse(i));
    EEPROM.get(adresse(i) + 1, e.secondes);
    return e;
}

void effacer_enregistrements() {
    nombre = 0;
    EEPROM.update(ADRESSE_NOMBRE, 0);
    EEPROM.update(ADRESSE_CONTROLE, VALEUR_CONTROLE);
}

void formater_enregistrement(const Enregistrement& e, char* texte) {
    const RtcDateTime date(e.secondes);

    utoa(e.bpm, texte, 10);
    char* p = texte + strlen(texte);
    strcpy(p, " - ");
    p += 3;

    ecrire_deux_chiffres(p, date.Hour());
    p[2] = ':';
    ecrire_deux_chiffres(p + 3, date.Minute());
    p[5] = ':';
    ecrire_deux_chiffres(p + 6, date.Second());
    p += 8;

    strcpy(p, " - ");
    p += 3;

    ecrire_deux_chiffres(p, date.Day());
    p[2] = '/';
    ecrire_deux_chiffres(p + 3, date.Month());
    p[5] = '/';
    ecrire_deux_chiffres(p + 6, date.Year() % 100);
    p[8] = '\0';
}
