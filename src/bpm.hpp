#ifndef BPM_HPP
#define BPM_HPP

#include <Arduino.h>
#include "setup.hpp"

// 25 mesures par seconde.
const unsigned long PERIODE_LECTURE = 40;
const uint8_t NB_ECHANTILLONS = 25; // Environ une seconde de signal.
const int SEUIL_BATTEMENT = 10; // Unites ADC, a ajuster au capteur.
const unsigned long INTERVALLE_MIN = 300;  // 200 BPM maximum.
const unsigned long INTERVALLE_MAX = 2000; // 30 BPM minimum.

void init_bpm();
void actualiser_bpm();
float lire_bpm();

#endif
