#ifndef GUI_HPP
#define GUI_HPP

#include <stdint.h>

constexpr uint8_t GEN_MENU_SIZE = 3;

// selected : 0, 1 ou 2. Un indice invalide affiche le premier choix.
// Dessine le menu complet ; aucune action ni lecture de l'encodeur.
void print_gen_menu(uint8_t selected);

// Affiche uniquement "bonjour" au centre du deuxieme ecran.
void print_hello_screen();

#endif
