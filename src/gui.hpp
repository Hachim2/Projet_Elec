#ifndef GUI_HPP
#define GUI_HPP

#include <stdint.h>

constexpr uint8_t GEN_MENU_SIZE = 3;

// selected : 0, 1 ou 2. Un indice invalide affiche le premier choix.
// Dessine le menu complet ; aucune action ni lecture de l'encodeur.
void print_gen_menu(uint8_t selected);

// Graphique BPM (ecran 1) : BPM en ordonnee, t(s) depuis le demarrage en
// abscisse. Un point d'historique toutes les GRAPHE_PERIODE_MS.
constexpr uint16_t GRAPHE_PERIODE_MS = 500;

// Enregistre un point (0 = pas de mesure), meme si le graphe est cache.
void graphe_ajouter_point(uint8_t bpm);

// pas > 0 : zoom avant sur l'axe du temps, pas < 0 : zoom arriere.
// Renvoie true si le niveau de zoom a change.
bool graphe_zoomer(int pas);

void afficher_graphe_bpm();

// Page "Mes donnees" : la plus recente mesure en haut, puis "Effacer tout".
// Le nombre de lignes selectionnables vaut 0 s'il n'y a aucune mesure.
uint8_t nombre_lignes_donnees();
void afficher_donnees(uint8_t selection);

// Titre (texte en Flash, avec PSTR) et une ligne de detail.
void afficher_message(const char* titre_flash, const char* detail);

void afficher_confirmation_effacement();

// Affiche le BPM en grand sur le deuxieme ecran ("--" si bpm vaut 0).
void print_bpm_screen(uint8_t bpm);

#endif
