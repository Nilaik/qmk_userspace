#include QMK_KEYBOARD_H
#include "action_tapping.h"
#include "gpio.h"

static uint8_t held_home_mods[2];
static uint8_t applied_home_mods;

static bool is_home_row_mod(uint16_t keycode) {
    return IS_QK_MOD_TAP(keycode) && keycode != LCTL_T(KC_Z);
}

bool get_permissive_hold(uint16_t keycode, keyrecord_t *record) {
    return is_home_row_mod(keycode);
}

bool get_chordal_hold(uint16_t tap_hold_keycode, keyrecord_t *tap_hold_record, uint16_t other_keycode, keyrecord_t *other_record) {
    return !is_home_row_mod(tap_hold_keycode) || get_chordal_hold_default(tap_hold_record, other_record);
}

static uint8_t mod_mask(uint8_t mods) {
    return (mods & 0x10) ? (mods & 0x0F) << 4 : mods;
}

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    if (record->event.type != KEY_EVENT) {
        return true;
    }

    const bool right = chordal_hold_handedness(record->event.key) == 'R';
    if (is_home_row_mod(keycode) && !record->tap.count) {
        const uint8_t mods = mod_mask(QK_MOD_TAP_GET_MODS(keycode));
        if (record->event.pressed) {
            held_home_mods[right] |= mods;
        } else {
            held_home_mods[right] &= ~mods;
            send_keyboard_report();
        }
        return false;
    }

    if (record->event.pressed) {
        uint16_t tap_key = keycode;
        uint8_t key_mods = 0;
        if (IS_QK_MOD_TAP(keycode) && record->tap.count) {
            tap_key = QK_MOD_TAP_GET_TAP_KEYCODE(keycode);
        } else if (IS_QK_LAYER_TAP(keycode) && record->tap.count) {
            tap_key = QK_LAYER_TAP_GET_TAP_KEYCODE(keycode);
        } else if (IS_QK_MODS(keycode)) {
            tap_key = QK_MODS_GET_BASIC_KEYCODE(keycode);
            key_mods = mod_mask(QK_MODS_GET_MODS(keycode));
        }
        if (IS_BASIC_KEYCODE(tap_key) && tap_key > KC_TRNS && !IS_MODIFIER_KEYCODE(tap_key)) {
            applied_home_mods = held_home_mods[!right] & ~(get_weak_mods() | key_mods);
            add_weak_mods(applied_home_mods);
        }
    }
    return true;
}

void post_process_record_user(uint16_t keycode, keyrecord_t *record) {
    // Keep the modifier in the keydown report, not in unrelated later reports.
    del_weak_mods(applied_home_mods);
    applied_home_mods = 0;
}

void keyboard_pre_init_user(void) {
    gpio_set_pin_output(24);
    gpio_write_pin_high(24);
}
