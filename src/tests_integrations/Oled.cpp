#include <Arduino.h>
#include <Wire.h>
#include <U8g2lib.h>

// Deux OLED SSD1306 128x64 sur le meme bus : SDA = A4, SCL = A5.
// Ces adresses sur 7 bits doivent correspondre au reglage des modules.
constexpr uint8_t ADRESSE_OLED_1 = 0x3D;
constexpr uint8_t ADRESSE_OLED_2 = 0x3C;

// Le mode "1" utilise un tampon d'une page pour economiser la RAM du Nano.
U8G2_SSD1306_128X64_NONAME_1_HW_I2C ecran1(U8G2_R0, U8X8_PIN_NONE);
U8G2_SSD1306_128X64_NONAME_1_HW_I2C ecran2(U8G2_R0, U8X8_PIN_NONE);

void afficher_bonjour(U8G2& ecran) {
    const char texte[] = "BONJOUR";
    ecran.setFont(u8g2_font_ncenB14_tr);
    ecran.setFontPosCenter();
    ecran.setDrawColor(1);

    const int16_t x = (ecran.getDisplayWidth() - ecran.getStrWidth(texte)) / 2;
    const int16_t y = ecran.getDisplayHeight() / 2;

    ecran.firstPage();
    do {
        ecran.drawStr(x, y, texte);
    } while (ecran.nextPage());
}

void setup() {
    Wire.begin();

    // U8g2 attend l'adresse I2C multipliee par deux, avant begin().
    ecran1.setI2CAddress(ADRESSE_OLED_1 << 1);
    ecran1.begin();
    ecran2.setI2CAddress(ADRESSE_OLED_2 << 1);
    ecran2.begin();

    afficher_bonjour(ecran1);
    afficher_bonjour(ecran2);
}

void loop() {
    // L'image reste affichee apres son envoi aux ecrans.
}
