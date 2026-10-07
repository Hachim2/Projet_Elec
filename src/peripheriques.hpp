#ifndef PERIPHERIQUES_HPP
#define PERIPHERIQUES_HPP

#include <stdint.h>

// Buzzer, LEDs de zone et bouton retour. Broches definies dans setup.hpp.
void init_peripheriques();

// Bip court, sauf si le son est coupe dans les reglages. Hauteur selon la
// zone : grave < 60 BPM, moyenne de 60 a 90 (ou BPM inconnu), aigue > 90.
void bip_buzzer(uint8_t bpm);

// 0 : tout eteint, < 60 : jaune, 60 a 90 : verte, > 90 : rouge.
// Tout eteint aussi si les LEDs sont coupees dans les reglages.
void afficher_zone_bpm(uint8_t bpm);

// Renvoie true une fois au relachement du bouton retour (rebonds filtres).
bool lire_bouton_retour();

#endif
