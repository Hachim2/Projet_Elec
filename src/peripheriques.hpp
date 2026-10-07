#ifndef PERIPHERIQUES_HPP
#define PERIPHERIQUES_HPP

#include <stdint.h>

// Buzzer, LEDs de zone, boutons retour et son. Broches definies dans setup.hpp.
void init_peripheriques();

// Bip court, sauf si le son est coupe (bouton D13). Hauteur selon la
// zone : grave < 60 BPM, moyenne de 60 a 100 (ou BPM inconnu), aigue > 100.
void bip_buzzer(uint8_t bpm);

// 0 : tout eteint, < 60 : jaune, 60 a 100 : verte, > 100 : rouge.
// Tout eteint aussi si les LEDs sont coupees dans les reglages.
void afficher_zone_bpm(uint8_t bpm);

// Renvoie true une fois au relachement du bouton retour (rebonds filtres).
bool lire_bouton_retour();

// A appeler dans loop() : chaque appui sur le bouton son (D13) coupe ou
// remet le bip, et le choix est garde dans l'EEPROM.
void actualiser_bouton_son();

#endif
