#include <Arduino.h>
#include "setup.hpp"
#include "gui.hpp"
#include "encoder.hpp"
#include "bpm.hpp"
#include "peripheriques.hpp"
#include "donnees.hpp"

// Passer a -1 si le zoom marche a l'envers (droite = dezoom).
static constexpr int8_t SENS_ZOOM = 1;
// Duree d'affichage d'un message avant le retour automatique au menu.
static constexpr unsigned long DUREE_MESSAGE_MS = 2500;
// Lecture du RTC plusieurs fois par seconde pour ne sauter aucune seconde.
static constexpr unsigned long PERIODE_HEURE_MS = 200;
// Rafraichissement du trace du signal : 20 images par seconde.
static constexpr unsigned long PERIODE_TRACE_MS = 50;

// Entrees du menu principal, dans l'ordre de GEN_MENU (gui.cpp).
enum ChoixMenu : uint8_t { GRAPHIQUE = 0, MES_DONNEES = 1, ENREGISTRER = 2 };

enum class Page : uint8_t { Menu, Graphe, Donnees, ConfirmerEffacement, Message };

static Page page = Page::Menu;
static int gen_selected = GRAPHIQUE;
static int previous_selected = GRAPHIQUE;
static int selection_donnees = 0;
static unsigned long dernier_trace = 0;
static unsigned long debut_message = 0;
static unsigned long derniere_lecture_heure = 0;
// Valeur impossible pour forcer le premier affichage.
static uint8_t bpm_affiche = 255;
static char heure_affichee[TAILLE_TEXTE_HEURE] = "";

static void ouvrir_menu() {
    page = Page::Menu;
    print_gen_menu(gen_selected);
    previous_selected = gen_selected;
}

static void ouvrir_donnees() {
    page = Page::Donnees;
    selection_donnees = 0;
    afficher_donnees(selection_donnees);
}

static void ouvrir_message(PGM_P titre, const char* detail) {
    page = Page::Message;
    afficher_message(titre, detail);
    debut_message = millis();
}

static void enregistrer(uint8_t bpm) {
    Enregistrement e;
    char detail[TAILLE_TEXTE_ENREGISTREMENT];

    switch (enregistrer_bpm(bpm, &e)) {
    case Resultat::Ok:
        formater_enregistrement(e, detail);
        ouvrir_message(PSTR("Enregistre !"), detail);
        break;
    case Resultat::PasDeBpm:
        strcpy_P(detail, PSTR("Poser le doigt"));
        ouvrir_message(PSTR("Pas de BPM"), detail);
        break;
    case Resultat::HorlogeInvalide:
        strcpy_P(detail, PSTR("Regler via port serie"));
        ouvrir_message(PSTR("Horloge invalide"), detail);
        break;
    case Resultat::MemoirePleine:
        strcpy_P(detail, PSTR("Effacer Mes donnees"));
        ouvrir_message(PSTR("Memoire pleine"), detail);
        break;
    case Resultat::ErreurEcriture:
        strcpy_P(detail, PSTR("Relecture differente"));
        ouvrir_message(PSTR("Erreur EEPROM"), detail);
        break;
    }
}

static void valider_menu(uint8_t bpm) {
    switch (gen_selected) {
    case GRAPHIQUE:
        page = Page::Graphe;
        afficher_graphe_signal(bpm);
        dernier_trace = millis();
        break;
    case MES_DONNEES:
        ouvrir_donnees();
        break;
    case ENREGISTRER:
        enregistrer(bpm);
        break;
    }
}

void setup() {
    Serial.begin(115200);
    Serial.println(F("Regler l'heure : envoyer AAAA-MM-JJ HH:MM:SS puis Entree"));
    init_hardware();
    init_peripheriques();
    init_donnees();
    init_bpm();
    init_graphe();
    ouvrir_menu();
}

void loop() {
    const float bpm = actualiser_bpm();
    const uint8_t bpm_entier = static_cast<uint8_t>(bpm + 0.5f);

    actualiser_reglage_serie();
    if (battement_detecte()) bip_buzzer();
    actualiser_buzzer();

    // Ecran 2 et LEDs : mis a jour seulement quand le BPM ou l'heure change.
    bool ecran2_a_jour = true;
    if (millis() - derniere_lecture_heure >= PERIODE_HEURE_MS) {
        derniere_lecture_heure = millis();
        char heure[TAILLE_TEXTE_HEURE];
        formater_heure_actuelle(heure);
        if (strcmp(heure, heure_affichee) != 0) {
            strcpy(heure_affichee, heure);
            ecran2_a_jour = false;
        }
    }
    if (bpm_entier != bpm_affiche) {
        afficher_zone_bpm(bpm_entier);
        bpm_affiche = bpm_entier;
        ecran2_a_jour = false;
    }
    if (!ecran2_a_jour) print_bpm_screen(bpm_affiche, heure_affichee);

    // Le trace se remplit en permanence, graphe affiche ou non.
    int8_t echantillon;
    while (lire_echantillon_signal(&echantillon)) {
        graphe_ajouter_echantillon(echantillon);
    }

    // Lu a chaque tour pour que l'anti-rebond suive le bouton.
    const bool retour = lire_bouton_retour();

    switch (page) {
    case Page::Menu:
        if (lire_encodeur(&gen_selected, GEN_MENU_SIZE)) {
            valider_menu(bpm_entier);
        } else if (gen_selected != previous_selected) {
            // Le menu reste affiche sans devoir le renvoyer en permanence.
            print_gen_menu(gen_selected);
            previous_selected = gen_selected;
        }
        break;

    case Page::Graphe: {
        // Droite : zoom avant sur le temps, gauche : zoom arriere.
        const int8_t pas = lire_pas_encodeur();
        const bool zoom_change = pas != 0 && graphe_zoomer(SENS_ZOOM * pas);
        if (lire_bouton_encodeur() || retour) {
            ouvrir_menu();
        } else if (zoom_change || millis() - dernier_trace >= PERIODE_TRACE_MS) {
            dernier_trace = millis();
            afficher_graphe_signal(bpm_entier);
        }
        break;
    }

    case Page::Donnees: {
        const int8_t pas = lire_pas_encodeur();
        const bool clic = lire_bouton_encodeur();
        const uint8_t nb_lignes = nombre_lignes_donnees();
        if (retour) {
            ouvrir_menu();
        } else if (clic && nb_lignes > 0 && selection_donnees == nb_lignes - 1) {
            // Derniere ligne : "Effacer tout".
            page = Page::ConfirmerEffacement;
            afficher_confirmation_effacement();
        } else if (pas != 0 && nb_lignes > 0) {
            const int nouvelle = constrain(selection_donnees + pas, 0, nb_lignes - 1);
            if (nouvelle != selection_donnees) {
                selection_donnees = nouvelle;
                afficher_donnees(selection_donnees);
            }
        }
        break;
    }

    case Page::ConfirmerEffacement:
        lire_pas_encodeur(); // Rotations ignorees.
        if (lire_bouton_encodeur()) {
            effacer_enregistrements();
            ouvrir_donnees();
        } else if (retour) {
            ouvrir_donnees();
        }
        break;

    case Page::Message:
        lire_pas_encodeur(); // Rotations ignorees.
        if (lire_bouton_encodeur() || retour ||
            millis() - debut_message >= DUREE_MESSAGE_MS) {
            ouvrir_menu();
        }
        break;
    }
}
