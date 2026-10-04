#include <Arduino.h>
#include "setup.hpp"
#include "gui.hpp"
#include "encoder.hpp"
#include "bpm.hpp"

// 0 = Choix 1, 1 = Choix 2, 2 = Choix 3.
int gen_selected = 0;
static int previous_selected = 0;
static bool graphe_actif = false;
static unsigned long dernierPoint = 0;
static constexpr unsigned long PERIODE_GRAPHE = 200;

static void do_SW2BTN() {
    if (graphe_actif) {
        graphe_actif = false;
        print_gen_menu(gen_selected);
    } else if (gen_selected == 0) {
        graphe_actif = true;
        reinitialiser_graphe_bpm();
        dernierPoint = millis() - PERIODE_GRAPHE;
    }
}

void setup() {
    init_hardware();
    init_bpm();
    print_gen_menu(gen_selected);
    print_hello_screen();
    previous_selected = gen_selected;
}

void loop() {
    actualiser_bpm();
    // Consomme les rotations sans modifier le choix pendant le graphique.
    int selection_graphe = gen_selected;
    if (lire_encodeur(graphe_actif ? &selection_graphe : &gen_selected, GEN_MENU_SIZE)) {
        do_SW2BTN();
    }

    if (graphe_actif) {
        const unsigned long maintenant = millis();
        if (maintenant - dernierPoint >= PERIODE_GRAPHE && lire_bpm() > 0) {
            dernierPoint = maintenant;
            afficherGrapheBPM(lire_bpm());
        }
        return;
    }

    // Le menu reste affiche sans devoir le renvoyer en permanence.
    if (gen_selected != previous_selected) {
        print_gen_menu(gen_selected);
        previous_selected = gen_selected;
    }
}
