#ifndef GUI_HPP
#define GUI_HPP

#include <stdint.h>

constexpr uint8_t GEN_MENU_SIZE = 4;

// selected : 0 a 3. Un indice invalide affiche le premier choix.
// Dessine le menu complet ; aucune action ni lecture de l'encodeur.
void print_gen_menu(uint8_t selected);

// Page "Reglages" : Son, LEDs, Contraste, avec leur valeur a droite.
constexpr uint8_t NB_REGLAGES = 3;
void afficher_reglages(uint8_t selection);

// Graphique (ecran 1) : trace du signal du capteur facon moniteur
// d'hopital, balayage de gauche a droite, temps en secondes en abscisse.

// Vide le trace (a appeler une fois dans setup).
void init_graphe();

// Ajoute un echantillon du signal (lire_echantillon_signal), meme si le
// graphe est cache, pour que le trace soit deja rempli a l'ouverture.
void graphe_ajouter_echantillon(int8_t valeur);

// pas > 0 : zoom avant sur l'axe du temps, pas < 0 : zoom arriere.
// Renvoie true si le niveau de zoom a change.
bool graphe_zoomer(int pas);

// Redessine le trace, avec le BPM en haut a gauche.
void afficher_graphe_signal(uint8_t bpm);

// Page "Mes donnees" : la plus recente mesure en haut, puis "Effacer tout".
// Le nombre de lignes selectionnables vaut 0 s'il n'y a aucune mesure.
uint8_t nombre_lignes_donnees();
void afficher_donnees(uint8_t selection);

// Titre (texte en Flash, avec PSTR) et une ligne de detail.
void afficher_message(const char* titre_flash, const char* detail);

void afficher_confirmation_effacement();

// Deuxieme ecran : heure et date en haut, BPM en grand ("--" si bpm vaut 0).
void print_bpm_screen(uint8_t bpm, const char* heure);

#endif
