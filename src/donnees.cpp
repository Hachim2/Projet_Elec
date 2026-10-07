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

// Retard approximatif entre la compilation et le demarrage de la carte (s).
constexpr uint32_t DECALAGE_TELEVERSEMENT_S = 28;
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

// Ecrit "a:b:c" ou "a/b/c" (8 caracteres, sans caractere de fin).
void ecrire_trois_nombres(char* texte, uint8_t a, uint8_t b, uint8_t c, char separateur) {
    ecrire_deux_chiffres(texte, a);
    texte[2] = separateur;
    ecrire_deux_chiffres(texte + 3, b);
    texte[5] = separateur;
    ecrire_deux_chiffres(texte + 6, c);
}

void regler_horloge(const RtcDateTime& date) {
    rtc.SetIsWriteProtected(false);
    rtc.SetTrickleChargeSettings(DS1302Tcr_Disabled); // Pas de recharge de la pile.
    rtc.SetDateTime(date);
    rtc.SetIsRunning(true);
}

// Ligne recue sur le port serie, en attente du retour a la ligne.
char ligne_serie[24];
uint8_t longueur_ligne = 0;

// Attend 14 chiffres dans l'ordre annee, mois, jour, heure, minute, seconde.
// Les separateurs sont libres : "2026-10-06 14:32:05" convient.
void traiter_ligne_serie() {
    uint8_t chiffres[14];
    uint8_t n = 0;
    for (uint8_t i = 0; i < longueur_ligne && n < 14; ++i) {
        if (isdigit(ligne_serie[i])) chiffres[n++] = ligne_serie[i] - '0';
    }

    auto nombre = [&](uint8_t debut, uint8_t taille) {
        uint16_t valeur = 0;
        for (uint8_t i = 0; i < taille; ++i) valeur = valeur * 10 + chiffres[debut + i];
        return valeur;
    };
    const uint16_t annee = nombre(0, 4);
    const uint8_t mois = nombre(4, 2);
    const uint8_t jour = nombre(6, 2);
    const uint8_t heure = nombre(8, 2);
    const uint8_t minute = nombre(10, 2);
    const uint8_t seconde = nombre(12, 2);

    if (n != 14 || annee < 2000 || annee > 2099 || mois < 1 || mois > 12 ||
        jour < 1 || jour > 31 || heure > 23 || minute > 59 || seconde > 59) {
        Serial.println(F("Format attendu : AAAA-MM-JJ HH:MM:SS"));
        return;
    }

    regler_horloge(RtcDateTime(annee, mois, jour, heure, minute, seconde));
    char texte[TAILLE_TEXTE_HEURE];
    formater_heure_actuelle(texte);
    Serial.print(F("Heure reglee : "));
    Serial.println(texte);
}

// "14:32:05" + separation + "06/10/26", termine par '\0'.
void ecrire_heure_date(char* texte, const RtcDateTime& date, const char* separation) {
    ecrire_trois_nombres(texte, date.Hour(), date.Minute(), date.Second(), ':');
    strcpy(texte + 8, separation);
    char* p = texte + 8 + strlen(separation);
    ecrire_trois_nombres(p, date.Day(), date.Month(), date.Year() % 100, '/');
    p[8] = '\0';
}
}

void init_donnees() {
    rtc.Begin();

    // Horloge jamais reglee (ou pile vide) : on part de l'heure de compilation,
    // approximative. L'heure exacte se regle ensuite par le port serie.
    if (rtc.GetIsWriteProtected()) rtc.SetIsWriteProtected(false);
    if (!rtc.GetIsRunning() || !rtc.IsDateTimeValid()) {
        regler_horloge(RtcDateTime(__DATE__, __TIME__) + DECALAGE_TELEVERSEMENT_S);
    }

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

    // Relecture : la mesure n'est ajoutee a la liste que si l'EEPROM
    // contient exactement ce qui vient d'etre ecrit.
    uint32_t secondes_relues;
    EEPROM.get(adresse(nombre) + 1, secondes_relues);
    if (EEPROM.read(adresse(nombre)) != e.bpm || secondes_relues != e.secondes) {
        return Resultat::ErreurEcriture;
    }

    // Le compteur est ecrit en dernier : une coupure pendant l'ecriture ne
    // laisse pas de mesure a moitie ecrite dans la liste.
    EEPROM.update(ADRESSE_NOMBRE, nombre + 1);
    if (EEPROM.read(ADRESSE_NOMBRE) != nombre + 1) {
        return Resultat::ErreurEcriture;
    }
    ++nombre;

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
    utoa(e.bpm, texte, 10);
    char* p = texte + strlen(texte);
    strcpy(p, " - ");
    ecrire_heure_date(p + 3, RtcDateTime(e.secondes), " - ");
}

bool formater_heure_actuelle(char* texte) {
    if (!rtc.GetIsRunning() || !rtc.IsDateTimeValid()) {
        strcpy_P(texte, PSTR("--:--:--   --/--/--"));
        return false;
    }
    ecrire_heure_date(texte, rtc.GetDateTime(), "   ");
    return true;
}

void actualiser_reglage_serie() {
    while (Serial.available() > 0) {
        const char c = Serial.read();
        if (c == '\r' || c == '\n') {
            if (longueur_ligne > 0) {
                Serial.println();
                traiter_ligne_serie();
                longueur_ligne = 0;
            }
        } else if (longueur_ligne < sizeof(ligne_serie)) {
            ligne_serie[longueur_ligne++] = c;
            Serial.write(c); // Echo : on voit ce qu'on tape.
        }
    }
}
