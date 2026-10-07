#include "gui.hpp"
#include "setup.hpp"
#include "donnees.hpp"
#include "bpm.hpp"
#include <avr/pgmspace.h>
#include <string.h>

// Textes en Flash, comme dans le projet fourni. ASCII sans accents.
static const char menu_title[] PROGMEM = "Menu principal";
static const char bpm_text[] PROGMEM = "BPM";
static const char gen_0[] PROGMEM = "Graphique";
static const char gen_1[] PROGMEM = "Mes donnees";
static const char gen_2[] PROGMEM = "Enregistrer data";
static const char gen_3[] PROGMEM = "Reglages";
static const char* const GEN_MENU[] PROGMEM = {gen_0, gen_1, gen_2, gen_3};
static_assert(sizeof(GEN_MENU) / sizeof(GEN_MENU[0]) == GEN_MENU_SIZE,
              "GEN_MENU_SIZE doit correspondre au nombre de libelles.");

// Page "Reglages", dans l'ordre de modifier_reglage().
static const char reglage_0[] PROGMEM = "Son";
static const char reglage_1[] PROGMEM = "LEDs";
static const char reglage_2[] PROGMEM = "Contraste";
static const char* const REGLAGES[] PROGMEM = {reglage_0, reglage_1, reglage_2};
static_assert(sizeof(REGLAGES) / sizeof(REGLAGES[0]) == NB_REGLAGES,
              "NB_REGLAGES doit correspondre au nombre de libelles.");

// 20 caracteres maximum par libelle avec la police de 6 pixels.
static constexpr uint8_t LABEL_SIZE = 21;

// --- Trace du signal, facon moniteur d'hopital ---
// Le trace balaie la zone de gauche a droite et recommence a gauche en
// effacant devant lui. Chaque colonne = moyenne de plusieurs echantillons
// de 5 ms : c'est ce nombre qui regle le zoom sur le temps.
static constexpr uint8_t ECHANTILLONS_PAR_COLONNE[] = {2, 3, 4, 6, 8};
static constexpr uint8_t NB_ZOOMS = sizeof(ECHANTILLONS_PAR_COLONNE);
static constexpr uint8_t PERIODE_ECHANTILLON_MS = 5;
static uint8_t zoom = 2; // 4 echantillons par colonne : 2,3 s a l'ecran.

// Zone de trace : graduation a gauche, axe vertical en x = G_X.
static constexpr uint8_t G_X = 18; // Place pour "-200".
static constexpr uint8_t G_HAUT = 9;
static constexpr uint8_t G_BAS = 55;
static constexpr uint8_t Y_TEXTE_TEMPS = 58;
static constexpr uint8_t LARGEUR_TRACE = SCREEN_WIDTH - G_X - 1;
static constexpr uint8_t TROU_BALAYAGE = 6;

// Echelle verticale fixe, en unites ADC du signal recentre (capteur moins sa
// moyenne sur 0,5 s) : les pics montent au-dessus du seuil de detection
// (200), les creux descendent sous 0. Les valeurs en dehors sont collees
// au bord.
static constexpr int16_t SIGNAL_BAS = -200;
static constexpr int16_t SIGNAL_HAUT = 400;
static constexpr int16_t PAS_GRADUATION = 200;

static constexpr int8_t PAS_DE_DONNEE = -128;
static int8_t trace[LARGEUR_TRACE]; // Signal / DIVISEUR_SIGNAL_TRACE.
static uint8_t curseur = 0;         // Prochaine colonne ecrite.
static int16_t somme_colonne = 0;
static uint8_t nb_dans_colonne = 0;

static void effacer_trace() {
    memset(trace, PAS_DE_DONNEE, sizeof(trace));
    curseur = 0;
    somme_colonne = 0;
    nb_dans_colonne = 0;
}

void init_graphe() {
    effacer_trace();
}

void graphe_ajouter_echantillon(int8_t valeur) {
    somme_colonne += valeur;
    if (++nb_dans_colonne < ECHANTILLONS_PAR_COLONNE[zoom]) return;

    trace[curseur] = somme_colonne / nb_dans_colonne;
    somme_colonne = 0;
    nb_dans_colonne = 0;
    if (++curseur >= LARGEUR_TRACE) curseur = 0;
}

bool graphe_zoomer(int pas) {
    // pas > 0 : zoom avant, donc moins d'echantillons par colonne.
    const int nouveau = constrain((int)zoom - pas, 0, NB_ZOOMS - 1);
    if (nouveau == zoom) return false;
    zoom = nouveau;
    effacer_trace(); // L'echelle de temps change : on repart de zero.
    return true;
}

// Colonnes juste devant le curseur : effacees, comme sur un vrai moniteur.
static bool dans_le_trou(uint8_t colonne) {
    // Distance en avant du curseur, en repassant a gauche apres le bord droit.
    return (colonne + LARGEUR_TRACE - curseur) % LARGEUR_TRACE < TROU_BALAYAGE;
}

static int16_t y_signal(int16_t adc) {
    adc = constrain(adc, SIGNAL_BAS, SIGNAL_HAUT);
    return G_BAS - 1 - (int32_t)(adc - SIGNAL_BAS) * (G_BAS - 1 - G_HAUT) /
                           (SIGNAL_HAUT - SIGNAL_BAS);
}

void afficher_graphe_signal(uint8_t bpm) {
    const uint16_t ms_par_colonne = ECHANTILLONS_PAR_COLONNE[zoom] * PERIODE_ECHANTILLON_MS;

    // "72 BPM" a gauche, largeur de la fenetre a droite ("2.3s").
    char texte_bpm[8];
    if (bpm == 0) {
        strcpy_P(texte_bpm, PSTR("--"));
    } else {
        utoa(bpm, texte_bpm, 10);
    }
    strcat_P(texte_bpm, PSTR(" BPM"));

    const uint16_t dixiemes = ((uint32_t)ms_par_colonne * LARGEUR_TRACE + 50) / 100;
    char texte_zoom[8];
    utoa(dixiemes / 10, texte_zoom, 10);
    char* p = texte_zoom + strlen(texte_zoom);
    *p++ = '.';
    *p++ = '0' + dixiemes % 10;
    *p++ = 's';
    *p = '\0';

    display.setFont(u8g2_font_4x6_tr);
    display.setFontPosTop();
    display.setFontMode(0);
    display.setDrawColor(1);

    char texte[6];
    display.firstPage();
    do {
        display.drawStr(G_X + 2, 0, texte_bpm);
        display.drawStr(SCREEN_WIDTH - display.getStrWidth(texte_zoom), 0, texte_zoom);

        // Axes.
        display.drawVLine(G_X, G_HAUT, G_BAS - G_HAUT + 1);
        display.drawHLine(G_X, G_BAS, SCREEN_WIDTH - G_X);

        // Graduation fixe a gauche : -200, 0, 200 (seuil de detection), 400.
        for (int16_t v = SIGNAL_BAS; v <= SIGNAL_HAUT; v += PAS_GRADUATION) {
            const int16_t y = y_signal(v);
            itoa(v, texte, 10);
            display.drawStr(G_X - 2 - display.getStrWidth(texte), y - 2, texte);
            display.drawPixel(G_X - 1, y);
            for (uint8_t x = G_X + 4; x < SCREEN_WIDTH; x += 4) display.drawPixel(x, y);
        }

        // Graduations toutes les secondes depuis le bord gauche du balayage.
        for (uint8_t s = 0;; ++s) {
            const uint16_t colonne = (uint32_t)s * 1000 / ms_par_colonne;
            if (colonne >= LARGEUR_TRACE) break;
            const uint8_t x = G_X + 1 + colonne;
            display.drawVLine(x, G_BAS + 1, 2);
            for (uint8_t y = G_HAUT; y < G_BAS; y += 4) display.drawPixel(x, y);
            utoa(s, texte, 10);
            strcat_P(texte, PSTR("s"));
            display.drawStr(s == 0 ? G_X + 1 : x - display.getStrWidth(texte) / 2,
                            Y_TEXTE_TEMPS, texte);
        }

        // Trace : un segment entre chaque colonne voisine, sauf dans le trou.
        for (uint8_t c = 1; c < LARGEUR_TRACE; ++c) {
            const int8_t a = trace[c - 1];
            const int8_t b = trace[c];
            if (a == PAS_DE_DONNEE || b == PAS_DE_DONNEE) continue;
            if (dans_le_trou(c) || dans_le_trou(c - 1)) continue;
            display.drawLine(G_X + c, y_signal(a * DIVISEUR_SIGNAL_TRACE),
                             G_X + 1 + c, y_signal(b * DIVISEUR_SIGNAL_TRACE));
        }
    } while (display.nextPage());
}

static void load_label(PGM_P source, char* buffer) {
    strncpy_P(buffer, source, LABEL_SIZE - 1);
    buffer[LABEL_SIZE - 1] = '\0';
}

static void print_center_line(const char* text, uint8_t y) {
    const int16_t x = (SCREEN_WIDTH - display.getStrWidth(text)) / 2;
    display.drawStr(x, y, text);
}

// Valeur affichee a droite d'un reglage ("Oui", "Fort"...), en Flash.
static PGM_P valeur_reglage(uint8_t i) {
    switch (i) {
    case 0: return reglages.son ? PSTR("Oui") : PSTR("Non");
    case 1: return reglages.leds ? PSTR("Oui") : PSTR("Non");
    default:
        if (reglages.contraste == CONTRASTE_FAIBLE) return PSTR("Faible");
        if (reglages.contraste == CONTRASTE_MOYEN) return PSTR("Moyen");
        return PSTR("Fort");
    }
}

// Titre puis une ligne par libelle (4 au maximum), la selection surlignee.
// Pour la page "Reglages", la valeur de chaque ligne est ecrite a droite.
static void draw_liste(PGM_P titre, const char* const* libelles, uint8_t nb,
                       uint8_t selected, bool avec_valeurs) {
    char text[LABEL_SIZE];
    display.setFont(u8g2_font_6x12_tf);
    display.setFontPosTop();
    display.setFontMode(1); // Fond transparent pour le texte surligne.
    display.setDrawColor(1);

    load_label(titre, text);
    print_center_line(text, 0);
    display.drawHLine(0, 13, SCREEN_WIDTH);

    for (uint8_t i = 0; i < nb; ++i) {
        const uint8_t y = 16 + i * 12;
        load_label(reinterpret_cast<PGM_P>(pgm_read_ptr(&libelles[i])), text);

        if (i == selected) {
            display.drawBox(0, y, SCREEN_WIDTH, 12);
            display.setDrawColor(0); // Texte noir sur fond blanc.
        }
        display.drawStr(4, y, text);
        if (avec_valeurs) {
            load_label(valeur_reglage(i), text);
            display.drawStr(SCREEN_WIDTH - 4 - display.getStrWidth(text), y, text);
        }
        display.setDrawColor(1);
    }
}

void print_gen_menu(uint8_t selected) {
    if (selected >= GEN_MENU_SIZE) selected = 0;

    display.firstPage();
    do {
        // Meme image pour toutes les pages : pas de lecture de capteur ici.
        draw_liste(menu_title, GEN_MENU, GEN_MENU_SIZE, selected, false);
    } while (display.nextPage());
}

void afficher_reglages(uint8_t selection) {
    display.firstPage();
    do {
        draw_liste(PSTR("Reglages"), REGLAGES, NB_REGLAGES, selection, true);
    } while (display.nextPage());
}

// --- Page "Mes donnees" ---
// 6 lignes de 9 pixels sous le titre ; la plus recente mesure en haut,
// "Effacer tout" en derniere ligne.
static constexpr uint8_t LIGNES_DONNEES = 6;
static constexpr uint8_t HAUTEUR_LIGNE = 9;
static constexpr uint8_t Y_PREMIERE_LIGNE = 10;
static uint8_t premiere_ligne = 0;

uint8_t nombre_lignes_donnees() {
    const uint8_t n = nombre_enregistrements();
    return n == 0 ? 0 : n + 1;
}

void afficher_donnees(uint8_t selection) {
    const uint8_t n = nombre_enregistrements();
    const uint8_t nb_lignes = nombre_lignes_donnees();

    // Faire defiler la liste pour garder la selection visible.
    if (selection < premiere_ligne) premiere_ligne = selection;
    if (selection >= premiere_ligne + LIGNES_DONNEES) {
        premiere_ligne = selection - LIGNES_DONNEES + 1;
    }
    if (premiere_ligne + LIGNES_DONNEES > nb_lignes) {
        premiere_ligne = nb_lignes > LIGNES_DONNEES ? nb_lignes - LIGNES_DONNEES : 0;
    }

    char titre[22];
    strcpy_P(titre, PSTR("Mes donnees ("));
    utoa(n, titre + strlen(titre), 10);
    strcat_P(titre, PSTR("/"));
    utoa(NB_MAX_ENREGISTREMENTS, titre + strlen(titre), 10);
    strcat_P(titre, PSTR(")"));

    display.setFont(u8g2_font_5x7_tr);
    display.setFontPosTop();
    display.setFontMode(1);
    display.setDrawColor(1);

    char texte[TAILLE_TEXTE_ENREGISTREMENT];
    display.firstPage();
    do {
        display.drawStr(0, 0, titre);
        display.drawHLine(0, 8, SCREEN_WIDTH);

        if (n == 0) {
            strcpy_P(texte, PSTR("Aucune donnee"));
            print_center_line(texte, 30);
            continue;
        }

        for (uint8_t l = 0; l < LIGNES_DONNEES; ++l) {
            const uint8_t i = premiere_ligne + l;
            if (i >= nb_lignes) break;

            if (i < n) {
                formater_enregistrement(lire_enregistrement(n - 1 - i), texte);
            } else {
                strcpy_P(texte, PSTR("Effacer tout"));
            }

            const uint8_t y = Y_PREMIERE_LIGNE + l * HAUTEUR_LIGNE;
            if (i == selection) {
                display.drawBox(0, y, SCREEN_WIDTH, HAUTEUR_LIGNE);
                display.setDrawColor(0);
            }
            display.drawStr(1, y + 1, texte);
            display.setDrawColor(1);
        }
    } while (display.nextPage());
}

// Titre en grand, une ligne de detail en petit en dessous.
void afficher_message(PGM_P titre, const char* detail) {
    char texte[LABEL_SIZE];
    load_label(titre, texte);

    display.setFontPosTop();
    display.setFontMode(1);
    display.setDrawColor(1);
    display.firstPage();
    do {
        display.setFont(u8g2_font_6x12_tf);
        print_center_line(texte, 16);
        display.setFont(u8g2_font_5x7_tr);
        print_center_line(detail, 36);
    } while (display.nextPage());
}

void afficher_confirmation_effacement() {
    char texte[LABEL_SIZE];
    display.setFontPosTop();
    display.setFontMode(1);
    display.setDrawColor(1);
    display.firstPage();
    do {
        display.setFont(u8g2_font_6x12_tf);
        strcpy_P(texte, PSTR("Effacer tout ?"));
        print_center_line(texte, 10);
        display.setFont(u8g2_font_5x7_tr);
        strcpy_P(texte, PSTR("Clic encodeur : oui"));
        print_center_line(texte, 34);
        strcpy_P(texte, PSTR("Bouton retour : non"));
        print_center_line(texte, 46);
    } while (display.nextPage());
}

void print_bpm_screen(uint8_t bpm, const char* heure) {
    char valeur[4];
    if (bpm == 0) {
        strcpy(valeur, "--");
    } else {
        utoa(bpm, valeur, 10);
    }
    char unite[LABEL_SIZE];
    load_label(bpm_text, unite);

    display_second.setFontPosTop();
    display_second.setDrawColor(1);
    display_second.setFont(u8g2_font_logisoso32_tn);
    const int16_t x_valeur = (SCREEN_WIDTH - display_second.getStrWidth(valeur)) / 2;
    display_second.setFont(u8g2_font_6x12_tf);
    const int16_t x_unite = (SCREEN_WIDTH - display_second.getStrWidth(unite)) / 2;
    display_second.setFont(u8g2_font_5x7_tr);
    const int16_t x_heure = (SCREEN_WIDTH - display_second.getStrWidth(heure)) / 2;

    // Heure en haut, BPM en grand au milieu, unite en bas.
    display_second.firstPage();
    do {
        display_second.setFont(u8g2_font_5x7_tr);
        display_second.drawStr(x_heure, 0, heure);
        display_second.drawHLine(0, 9, SCREEN_WIDTH);
        display_second.setFont(u8g2_font_logisoso32_tn);
        display_second.drawStr(x_valeur, 13, valeur);
        display_second.setFont(u8g2_font_6x12_tf);
        display_second.drawStr(x_unite, 51, unite);
    } while (display_second.nextPage());
}
