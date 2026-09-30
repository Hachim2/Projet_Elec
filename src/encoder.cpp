#include "encoder.hpp"
#include "setup.hpp"
#include <avr/interrupt.h>
#include <util/atomic.h>

namespace {
constexpr uint32_t BUTTON_DEBOUNCE_MS = 30;

uint8_t previous_ab = 0;
int8_t partial_step = 0;
volatile int8_t pending_steps = 0;

int last_button_reading = HIGH;
int stable_button = HIGH;
uint32_t last_button_change = 0;
bool pressed = false;

uint8_t read_encoder_ab() {
    return (digitalRead(SW2A) << 1) | digitalRead(SW2B);
}
}

// D3 et D4 du Nano partagent l'interruption PCINT2. La rotation reste lue
// pendant les transferts I2C ; aucun dessin ni delai dans l'interruption.
ISR(PCINT2_vect) {
    const uint8_t ab = read_encoder_ab();
    const uint8_t changed = previous_ab ^ ab;
    if (changed == 0) return;

    if (changed == 3) {
        // Deux bits ont change : sequence incomplete, on se resynchronise.
        partial_step = 0;
    } else {
        const bool forward = (previous_ab & 1) ^ ((ab >> 1) & 1);
        partial_step += forward ? 1 : -1;

        // Un cycle A/B complet = un pas. Les allers-retours dus aux rebonds
        // s'annulent avant le retour au repos (A et B a HIGH).
        if (ab == 3) {
            if (partial_step == 4 && pending_steps < 127) ++pending_steps;
            if (partial_step == -4 && pending_steps > -127) --pending_steps;
            partial_step = 0;
        }
    }
    previous_ab = ab;
}

void init_encodeur() {
    static_assert(digitalPinToPCICRbit(SW2A) == 2 &&
                  digitalPinToPCICRbit(SW2B) == 2,
                  "Cette lecture utilise les broches D0 a D7 du Nano.");

    pinMode(SW2A, INPUT_PULLUP);
    pinMode(SW2B, INPUT_PULLUP);
    pinMode(SW2BTN, INPUT_PULLUP);

    last_button_reading = digitalRead(SW2BTN);
    stable_button = last_button_reading;
    last_button_change = millis();
    pressed = false;

    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
        previous_ab = read_encoder_ab();
        partial_step = 0;
        pending_steps = 0;
        PCMSK2 |= _BV(digitalPinToPCMSKbit(SW2A)) |
                  _BV(digitalPinToPCMSKbit(SW2B));
        PCIFR = _BV(PCIF2);
        PCICR |= _BV(PCIE2);
    }
}

void apply_encoder_step(int delta, int* selected, int menu_size) {
    if (selected == nullptr || menu_size <= 0) return;

    const long next = static_cast<long>(*selected) + delta;
    *selected = next % menu_size;
    if (*selected < 0) *selected += menu_size;
}

bool lire_encodeur(int* selected, int menu_size) {
    int8_t delta;
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
        delta = pending_steps;
        pending_steps = 0;
    }
    apply_encoder_step(delta, selected, menu_size);

    const int btn = digitalRead(SW2BTN);
    const uint32_t now = millis();
    if (btn != last_button_reading) {
        last_button_reading = btn;
        last_button_change = now;
    }

    if (btn == stable_button || now - last_button_change < BUTTON_DEBOUNCE_MS) {
        return false;
    }

    stable_button = btn;
    if (btn == LOW) {
        pressed = true;
        return false;
    }

    const bool clicked = pressed;
    pressed = false;
    return clicked;
}
