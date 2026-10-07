#ifndef DONNEES_HPP
#define DONNEES_HPP

#include <stdint.h>

// Mesures sauvegardees dans l'EEPROM du Nano : elles restent apres une
// coupure d'alimentation. L'heure vient du RTC DS1302.
struct Enregistrement {
    uint8_t bpm;
    uint32_t secondes; // Secondes depuis le 01/01/2000 (format du RTC).
};

// 1 Ko d'EEPROM : 2 octets d'en-tete + 5 octets par mesure.
constexpr uint8_t NB_MAX_ENREGISTREMENTS = 200;

// Longueur de "120 - 14:32:05 - 06/10/26" + caractere de fin.
constexpr uint8_t TAILLE_TEXTE_ENREGISTREMENT = 26;

enum class Resultat : uint8_t {
    Ok,
    PasDeBpm,
    HorlogeInvalide,
    MemoirePleine,
    ErreurEcriture, // La relecture de l'EEPROM ne correspond pas.
};

void init_donnees();

// Sauvegarde le BPM avec la date et l'heure du RTC. Si tout va bien,
// copie la mesure dans *sauve (pour l'afficher en confirmation).
Resultat enregistrer_bpm(uint8_t bpm, Enregistrement* sauve);

uint8_t nombre_enregistrements();

// i = 0 : la plus ancienne mesure.
Enregistrement lire_enregistrement(uint8_t i);

void effacer_enregistrements();

// Format "bpm - heure - date" : "72 - 14:32:05 - 06/10/26".
void formater_enregistrement(const Enregistrement& e, char* texte);

// Heure et date du RTC : "14:32:05   06/10/26". Renvoie false (et des
// tirets) si l'horloge est arretee ou jamais reglee.
constexpr uint8_t TAILLE_TEXTE_HEURE = 20;
bool formater_heure_actuelle(char* texte);

// A appeler dans loop() : regle l'horloge quand on envoie sur le port
// serie (115200 bauds) une ligne "AAAA-MM-JJ HH:MM:SS" puis Entree.
void actualiser_reglage_serie();

#endif
