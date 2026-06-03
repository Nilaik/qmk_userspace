// Copyright 2026 Nilaik
// SPDX-License-Identifier: GPL-2.0-or-later

#include QMK_KEYBOARD_H

// Total number of layers we cycle through (0..LAYER_COUNT-1).
// The 3 RGB LEDs encode the active layer number in binary, so 3 bits -> 8 layers.
#define LAYER_COUNT 8

// How long (ms) key 0 must be held before it counts as a "hold" (jump to layer 0)
// instead of a "tap" (advance to the next layer).
#define LAYER_HOLD_TERM 200

enum custom_keycodes {
    CYCLE_LAYER = SAFE_RANGE, // tap: next layer, hold: back to layer 0
};

// Key 0 is CYCLE_LAYER on every layer so you can always cycle / reset.
// Keys 1 and 2 keep copy / paste here; customize them per layer as you like.
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [0] = LAYOUT(CYCLE_LAYER, LCTL(KC_C), LCTL(KC_V)),
    [1] = LAYOUT(CYCLE_LAYER, KC_TAB, KC_ENTER),
    [2] = LAYOUT(CYCLE_LAYER, KC_BACKSPACE, KC_DELETE),
    [3] = LAYOUT(CYCLE_LAYER, KC_SPACE, KC_ESCAPE),
    [4] = LAYOUT(CYCLE_LAYER, KC_NO, KC_NO),
    [5] = LAYOUT(CYCLE_LAYER, KC_NO, KC_NO),
    [6] = LAYOUT(CYCLE_LAYER, KC_NO, KC_NO),
    [7] = LAYOUT(CYCLE_LAYER, KC_NO, KC_NO),
};

static bool     cycle_pressed = false; // is key 0 currently held down?
static bool     hold_fired    = false; // did the current press already trigger the hold action?
static uint16_t cycle_timer   = 0;     // time key 0 was pressed

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
        case CYCLE_LAYER:
            if (record->event.pressed) {
                cycle_pressed = true;
                hold_fired    = false;
                cycle_timer   = timer_read();
            } else {
                cycle_pressed = false;
                // If the hold action already fired, the release does nothing.
                if (!hold_fired) {
                    // Tap: advance to the next layer, wrapping back to 0 after the last.
                    uint8_t next = (get_highest_layer(layer_state) + 1) % LAYER_COUNT;
                    layer_move(next);
                }
            }
            return false; // we fully handle this key
    }
    return true;
}

// Fires the "hold" action as soon as LAYER_HOLD_TERM elapses, while the key is
// still held, so going back to layer 0 feels immediate.
void matrix_scan_user(void) {
    if (cycle_pressed && !hold_fired && timer_elapsed(cycle_timer) >= LAYER_HOLD_TERM) {
        layer_move(0);
        hold_fired = true;
    }
}

// Display the active layer number in binary on the 3 RGB LEDs.
// LED index 0 = bit 0 (LSB), LED 1 = bit 1, LED 2 = bit 2.
// A set bit lights up; a clear bit is off. So layer 0 = all off, layer 7 = all on.
bool rgb_matrix_indicators_user(void) {
    uint8_t layer = get_highest_layer(layer_state);
    for (uint8_t i = 0; i < 3; i++) {
        if (layer & (1 << i)) {
            rgb_matrix_set_color(i, RGB_GREEN);
        } else {
            rgb_matrix_set_color(i, RGB_OFF);
        }
    }
    return false;
}
