#include "gui.hpp"
#include "setup.hpp"
#include "donnees.hpp"
#include <avr/pgmspace.h>
#include <string.h>

// Textes en Flash, comme dans le projet fourni. ASCII sans accents.
static const char menu_title[] PROGMEM = "Menu principal";
static const char bpm_text[] PROGMEM = "BPM";
static const char gen_0[] PROGMEM = "Graphique";
static const char gen_1[] PROGMEM = "Mes donnees";
static const char gen_2[] PROGMEM = "Enregistrer data";
static const char* const GEN_MENU[] PROGMEM = {gen_0, gen_1, gen_2};
static_assert(sizeof(GEN_MENU) / sizeof(GEN_MENU[0]) == GEN_MENU_SIZE,
              "GEN_MENU_SIZE doit correspondre au nombre de libelles.");

// 20 caracteres maximum par libelle avec la police de 6 pixels.
static constexpr uint8_t LABEL_SIZE = 21;

// --- Graphique BPM ---
// Historique : un point toutes les 500 ms, les 2 dernieres minutes.
// Le point d'indice k correspond a t = k / 2 secondes depuis le demarrage.
static constexpr uint8_t POINTS_PAR_SECONDE = 1000 / GRAPHE_PERIODE_MS;
static constexpr uint16_t TAILLE_HISTORIQUE = 120 * POINTS_PAR_SECONDE;
static uint8_t historique[TAILLE_HISTORIQUE]; // 0 = pas de mesure.
static uint32_t nb_points_total = 0;

// Niveaux de zoom : largeur de la fenetre et pas des graduations (s).
static constexpr uint8_t FENETRES_S[] = {5, 10, 20, 30, 60, 120};
static constexpr uint8_t GRADUATIONS_S[] = {1, 2, 5, 5, 10, 30};
static constexpr uint8_t NB_ZOOMS = sizeof(FENETRES_S);
static_assert(sizeof(GRADUATIONS_S) == NB_ZOOMS, "Un pas par niveau de zoom.");
static_assert(FENETRES_S[NB_ZOOMS - 1] * POINTS_PAR_SECONDE <= TAILLE_HISTORIQUE,
              "La plus grande fenetre doit tenir dans l'historique.");
static uint8_t zoom = 3; // 30 s au depart.

// Zone de trace : axe des BPM en x = 16, axe du temps en y = 55.
static constexpr uint8_t G_X = 16;
static constexpr uint8_t G_HAUT = 9;
static constexpr uint8_t G_BAS = 55;
static constexpr uint8_t G_LARGEUR = SCREEN_WIDTH - G_X - 1;
static constexpr uint8_t Y_TEXTE_TEMPS = 58;

void graphe_ajouter_point(uint8_t bpm) {
    historique[nb_points_total % TAILLE_HISTORIQUE] = bpm;
    ++nb_points_total;
}

bool graphe_zoomer(int pas) {
    // pas > 0 : zoom avant, donc fenetre plus courte.
    const int nouveau = constrain((int)zoom - pas, 0, NB_ZOOMS - 1);
    if (nouveau == zoom) return false;
    zoom = nouveau;
    return true;
}

static int16_t x_point(uint32_t k, uint32_t debut, uint16_t fenetre) {
    return G_X + 1 + (int32_t)(k - debut) * (G_LARGEUR - 1) / fenetre;
}

static int16_t y_bpm(int bpm, int bas, int haut) {
    return G_BAS - 1 - (int32_t)(bpm - bas) * (G_BAS - 1 - G_HAUT) / (haut - bas);
}

// Libelle a gauche de l'axe, ligne pointillee horizontale dans le graphe.
static void graduation_bpm(int bpm, int bas, int haut) {
    char texte[4];
    utoa(bpm, texte, 10);
    const int16_t y = y_bpm(bpm, bas, haut);
    display.drawStr(G_X - 2 - display.getStrWidth(texte), y - 3, texte);
    display.drawPixel(G_X - 1, y);
    for (uint8_t x = G_X + 3; x < SCREEN_WIDTH; x += 3) display.drawPixel(x, y);
}

// Libelle sous l'axe, ligne pointillee verticale dans le graphe.
static void graduation_temps(uint32_t k, uint32_t debut, uint16_t fenetre) {
    char texte[6];
    utoa(k / POINTS_PAR_SECONDE, texte, 10);
    const int16_t x = x_point(k, debut, fenetre);
    const int16_t largeur = display.getStrWidth(texte);
    const int16_t x_texte = constrain(x - largeur / 2, G_X + 1, SCREEN_WIDTH - largeur);
    display.drawStr(x_texte, Y_TEXTE_TEMPS, texte);
    display.drawVLine(x, G_BAS + 1, 2);
    for (uint8_t y = G_HAUT; y < G_BAS; y += 3) display.drawPixel(x, y);
}

void afficher_graphe_bpm() {
    // Le graphe se remplit de gauche a droite, puis defile.
    const uint16_t fenetre = FENETRES_S[zoom] * POINTS_PAR_SECONDE;
    const uint32_t debut = nb_points_total > fenetre ? nb_points_total - fenetre : 0;
    const uint16_t pas = GRADUATIONS_S[zoom] * POINTS_PAR_SECONDE;
    const uint32_t premiere_graduation = (debut + pas - 1) / pas * pas;

    // Echelle verticale automatique, arrondie a la dizaine, 20 BPM minimum.
    uint8_t mini = 255;
    uint8_t maxi = 0;
    for (uint32_t k = debut; k < nb_points_total; ++k) {
        const uint8_t v = historique[k % TAILLE_HISTORIQUE];
        if (v == 0) continue;
        if (v < mini) mini = v;
        if (v > maxi) maxi = v;
    }
    int bas = 60;
    int haut = 100;
    if (maxi > 0) {
        bas = max(0, (mini - 5) / 10 * 10);
        haut = (maxi + 14) / 10 * 10;
        if (haut - bas < 20) {
            bas = max(0, bas - 10);
            haut += 10;
        }
    }

    char texte_zoom[8] = "zoom ";
    utoa(FENETRES_S[zoom], texte_zoom + 5, 10);
    strcat(texte_zoom, "s");

    display.setFont(u8g2_font_4x6_tr);
    display.setFontPosTop();
    display.setFontMode(0);
    display.setDrawColor(1);

    display.firstPage();
    do {
        display.drawStr(0, 0, "BPM");
        display.drawStr(SCREEN_WIDTH - display.getStrWidth(texte_zoom), 0, texte_zoom);
        display.drawStr(0, Y_TEXTE_TEMPS, "t(s)");

        display.drawVLine(G_X, G_HAUT, G_BAS - G_HAUT + 1);
        display.drawHLine(G_X, G_BAS, SCREEN_WIDTH - G_X);

        graduation_bpm(bas, bas, haut);
        graduation_bpm((bas + haut) / 2, bas, haut);
        graduation_bpm(haut, bas, haut);
        for (uint32_t k = premiere_graduation; k <= debut + fenetre; k += pas) {
            graduation_temps(k, debut, fenetre);
        }

        // Courbe : interrompue la ou il n'y avait pas de mesure.
        bool precedent = false;
        int16_t x_prec = 0;
        int16_t y_prec = 0;
        for (uint32_t k = debut; k < nb_points_total; ++k) {
            const uint8_t v = historique[k % TAILLE_HISTORIQUE];
            if (v == 0) {
                precedent = false;
                continue;
            }
            const int16_t x = x_point(k, debut, fenetre);
            const int16_t y = y_bpm(v, bas, haut);
            if (precedent) {
                display.drawLine(x_prec, y_prec, x, y);
            } else {
                display.drawPixel(x, y);
            }
            x_prec = x;
            y_prec = y;
            precedent = true;
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

static void draw_menu_page(uint8_t selected) {
    char text[LABEL_SIZE];
    display.setFont(u8g2_font_6x12_tf);
    display.setFontPosTop();
    display.setFontMode(1); // Fond transparent pour le texte surligne.
    display.setDrawColor(1);

    load_label(menu_title, text);
    print_center_line(text, 0);
    display.drawHLine(0, 14, SCREEN_WIDTH);

    for (uint8_t i = 0; i < GEN_MENU_SIZE; ++i) {
        const uint8_t y = 18 + i * 15;
        load_label(reinterpret_cast<PGM_P>(pgm_read_ptr(&GEN_MENU[i])), text);

        if (i == selected) {
            display.drawBox(0, y, SCREEN_WIDTH, 14);
            display.setDrawColor(0); // Texte noir sur fond blanc.
        }
        display.drawStr(4, y + 1, text);
        display.setDrawColor(1);
    }
}

void print_gen_menu(uint8_t selected) {
    if (selected >= GEN_MENU_SIZE) selected = 0;

    display.firstPage();
    do {
        // Meme image pour toutes les pages : pas de lecture de capteur ici.
        draw_menu_page(selected);
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

void print_bpm_screen(uint8_t bpm) {
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

    display_second.firstPage();
    do {
        display_second.setFont(u8g2_font_logisoso32_tn);
        display_second.drawStr(x_valeur, 6, valeur);
        display_second.setFont(u8g2_font_6x12_tf);
        display_second.drawStr(x_unite, 48, unite);
    } while (display_second.nextPage());
}
