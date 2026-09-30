#include "setup.hpp"
#include "encoder.hpp"
#include <Wire.h>

U8G2_SSD1306_128X64_NONAME_1_HW_I2C display(U8G2_R0, OLED_RESET);
U8G2_SSD1306_128X64_NONAME_1_HW_I2C display_second(U8G2_R0, OLED_RESET);

void init_hardware() {
    init_encodeur();
    Wire.begin();
    // U8g2 attend une adresse decalee d'un bit, contrairement au scanner I2C.
    display.setI2CAddress(OLED_ADDRESS << 1);
    display.begin();

    // Meme bus SDA/SCL, mais adresse distincte pour le deuxieme ecran.
    display_second.setI2CAddress(OLED_SECOND_ADDRESS << 1);
    display_second.begin();
}
