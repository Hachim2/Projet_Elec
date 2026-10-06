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

// Renvoie true une fois par battement detecte (pour le bip du buzzer).
bool battement_detecte();

#endif
