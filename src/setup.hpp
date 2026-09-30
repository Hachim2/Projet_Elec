#ifndef SETUP_HPP
#define SETUP_HPP

#include <Arduino.h>
#include <U8g2lib.h>

// Base : Arduino Nano ATmega328P, comme le projet fourni.
// En I2C materiel sur cette carte, SDA et SCL sont imposes.
constexpr uint8_t OLED_SDA = A4;
constexpr uint8_t OLED_SCL = A5;
constexpr uint8_t OLED_RESET = U8X8_PIN_NONE;
// Adresses I2C sur 7 bits, a faire correspondre au reglage physique des modules.
constexpr uint8_t OLED_ADDRESS = 0x3D; // Premier ecran : menu.
constexpr uint8_t OLED_SECOND_ADDRESS = 0x3C; // Deuxieme ecran : bonjour.
static_assert(OLED_ADDRESS != OLED_SECOND_ADDRESS,
              "Les deux ecrans doivent avoir des adresses I2C differentes.");
constexpr uint8_t SCREEN_WIDTH = 128;
constexpr uint8_t SCREEN_HEIGHT = 64;

// Encodeur : A/CLK sur D3, B/DT sur D4, bouton sur D2, commun sur GND.
// Les trois entrees utilisent les resistances de rappel internes.
constexpr uint8_t SW2A = 3;
constexpr uint8_t SW2B = 4;
constexpr uint8_t SW2BTN = 2;

// Le "1" selectionne un buffer d'une page (128 octets pour cet ecran).
extern U8G2_SSD1306_128X64_NONAME_1_HW_I2C display;
extern U8G2_SSD1306_128X64_NONAME_1_HW_I2C display_second;

void init_hardware();

#endif
