#include <Arduino.h>
#include "setup.hpp"
#include "gui.hpp"
#include "encoder.hpp"

// 0 = Choix 1, 1 = Choix 2, 2 = Choix 3.
int gen_selected = 0;
static int previous_selected = 0;

static void do_SW2BTN() {
    // Clic detecte au relachement. Ajouter ici les actions du choix
    // gen_selected (0, 1 ou 2) lorsqu'elles seront definies.
}

void setup() {
    init_hardware();
    print_gen_menu(gen_selected);
    print_hello_screen();
    previous_selected = gen_selected;
}

void loop() {
    if (lire_encodeur(&gen_selected, GEN_MENU_SIZE)) {
        do_SW2BTN();
    }

    // Le menu reste affiche sans devoir le renvoyer en permanence.
    if (gen_selected != previous_selected) {
        print_gen_menu(gen_selected);
        previous_selected = gen_selected;
    }
}
