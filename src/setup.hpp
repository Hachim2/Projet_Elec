#ifndef SETUP_HPP
#define SETUP_HPP

#include <Arduino.h>
#include <U8g2lib.h>

//Pin capteur PPG
const int PIN_PPG = A0;

// Base : Arduino Nano ATmega328P, comme le projet fourni.
// En I2C materiel sur cette carte, SDA et SCL sont imposes.
constexpr uint8_t OLED_SDA = A4;
constexpr uint8_t OLED_SCL = A5;
constexpr uint8_t OLED_RESET = U8X8_PIN_NONE;
// Adresses I2C sur 7 bits, a faire correspondre au reglage physique des modules.
constexpr uint8_t OLED_ADDRESS = 0x3D; // Premier ecran : menu.
constexpr uint8_t OLED_SECOND_ADDRESS = 0x3C; // Deuxieme ecran : BPM.
static_assert(OLED_ADDRESS != OLED_SECOND_ADDRESS,
              "Les deux ecrans doivent avoir des adresses I2C differentes.");
constexpr uint8_t SCREEN_WIDTH = 128;
constexpr uint8_t SCREEN_HEIGHT = 64;

// Encodeur : A/CLK sur D3, B/DT sur D4, bouton sur D2, commun sur GND.
// Les trois entrees utilisent les resistances de rappel internes.
constexpr uint8_t SW2A = 3;
constexpr uint8_t SW2B = 4;
constexpr uint8_t SW2BTN = 2;

// Bouton retour au menu principal, entre D9 et GND (rappel interne).
// Si le bouton est cable vers 5V avec une resistance de tirage vers GND,
// passer BTN_RETOUR_APPUI a HIGH.
constexpr uint8_t BTN_RETOUR = 9;
constexpr uint8_t BTN_RETOUR_APPUI = LOW;

// Bouton son on/off sur D13. La LED de la carte (avec sa resistance) tire
// D13 vers GND : le rappel interne ne monte qu'a ~1,7 V, illisible. Le
// bouton est donc cable entre 5V et D13, avec 10 kOhm entre D13 et GND.
// La LED de la carte s'allume pendant l'appui.
constexpr uint8_t BTN_SON = 13;
constexpr uint8_t BTN_SON_APPUI = HIGH;

// Buzzer pilote par tone() : un bip a chaque battement, a 3 hauteurs.
// Un buzzer passif est necessaire pour entendre nettement les 3 hauteurs.
constexpr uint8_t BUZZER = 5;

// LEDs de zone : jaune < 60 BPM, verte 60-100 BPM, rouge > 100 BPM.
constexpr uint8_t LED_ROUGE = 8;
constexpr uint8_t LED_VERTE = 7;
constexpr uint8_t LED_JAUNE = 6;

// Horloge RTC DS1302 (liaison 3 fils), memes broches que rtc_test.cpp.
constexpr uint8_t RTC_CLK = 10;
constexpr uint8_t RTC_DAT = 11;
constexpr uint8_t RTC_CE = 12;

// Le "1" selectionne un buffer d'une page (128 octets pour cet ecran).
extern U8G2_SSD1306_128X64_NONAME_1_HW_I2C display;
extern U8G2_SSD1306_128X64_NONAME_1_HW_I2C display_second;

void init_hardware();

// Applique reglages.contraste aux deux ecrans.
void appliquer_contraste();

#endif
