#ifndef BPM_HPP
#define BPM_HPP

#include <Arduino.h>

// Lance l'echantillonnage du capteur PPG toutes les 5 ms par le Timer1.
// Les mesures continuent pendant les dessins I2C des ecrans.
void init_bpm();

// A appeler dans loop() : integre les battements detectes et renvoie le BPM
// moyen sur les 10 derniers intervalles (0 tant qu'aucun rythme n'est trouve).
float actualiser_bpm();

// Derniere valeur calculee par actualiser_bpm(), sans traitement.
float lire_bpm();

// Echantillons du signal recentre (un toutes les 5 ms), divises par
// DIVISEUR_SIGNAL_TRACE pour tenir sur un octet : +-508 en unites ADC.
// Renvoie false quand il n'y en a plus en attente.
constexpr uint8_t DIVISEUR_SIGNAL_TRACE = 4;
bool lire_echantillon_signal(int8_t* valeur);

// Renvoie true une fois par battement detecte (pour le bip du buzzer).
bool battement_detecte();

#endif
