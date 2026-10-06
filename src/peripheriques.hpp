#ifndef PERIPHERIQUES_HPP
#define PERIPHERIQUES_HPP

#include <stdint.h>

// Buzzer, LEDs de zone et bouton retour. Broches definies dans setup.hpp.
void init_peripheriques();

// Lance un bip court ; il s'arrete tout seul dans actualiser_buzzer().
void bip_buzzer();

// A appeler dans loop() : coupe le buzzer a la fin du bip, sans delay().
void actualiser_buzzer();

// 0 : tout eteint, < 60 : jaune, 60 a 90 : verte, > 90 : rouge.
void afficher_zone_bpm(uint8_t bpm);

// Renvoie true une fois au relachement du bouton retour (rebonds filtres).
bool lire_bouton_retour();

#endif
