#include "gui.hpp"
#include "setup.hpp"
#include <avr/pgmspace.h>
#include <string.h>

// Modifier ces trois libelles pour nommer les futures fonctions.
// Textes en Flash, comme dans le projet fourni. ASCII sans accents.
static const char menu_title[] PROGMEM = "Menu principal";
static const char hello_text[] PROGMEM = "bonjour";
static const char gen_0[] PROGMEM = "Graphique";
static const char gen_1[] PROGMEM = "Mes donnees";
static const char gen_2[] PROGMEM = "Choix 3";
static const char* const GEN_MENU[] PROGMEM = {gen_0, gen_1, gen_2};

// 20 caracteres maximum par libelle avec la police de 6 pixels.
static constexpr uint8_t LABEL_SIZE = 21;

// Conserve les points pour redessiner chaque page du petit buffer OLED.
static uint8_t graphe_y[SCREEN_WIDTH];
static uint8_t graphe_points = 0;

void reinitialiser_graphe_bpm() {
    graphe_points = 0;
    display.firstPage();
    do {
        // Une page vide efface le graphique precedent ou le menu.
    } while (display.nextPage());
}

void afficherGrapheBPM(float bpm) {
    bpm = constrain(bpm, 40.0f, 180.0f);
    const uint8_t y = map((int)bpm, 40, 180, SCREEN_HEIGHT - 1, 0);

    if (graphe_points >= SCREEN_WIDTH) graphe_points = 0;
    graphe_y[graphe_points++] = y;

    display.setDrawColor(1);
    display.firstPage();
    do {
        display.drawPixel(0, graphe_y[0]);
        for (uint8_t x = 1; x < graphe_points; ++x) {
            display.drawLine(x - 1, graphe_y[x - 1], x, graphe_y[x]);
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

void print_hello_screen() {
    char text[LABEL_SIZE];
    load_label(hello_text, text);
    display_second.setFont(u8g2_font_6x12_tf);
    display_second.setFontPosCenter();
    display_second.setDrawColor(1);

    const int16_t x = (SCREEN_WIDTH - display_second.getStrWidth(text)) / 2;
    display_second.firstPage();
    do {
        display_second.drawStr(x, SCREEN_HEIGHT / 2, text);
    } while (display_second.nextPage());
}
