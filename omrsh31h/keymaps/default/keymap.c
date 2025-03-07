#include QMK_KEYBOARD_H

#include "config.h"

// Combined enum for all custom keycodes
enum custom_keycodes {
    CC_SS = SAFE_RANGE,
    CC_DB
};

enum layers {
    _CURSOR,    // Layer 0: Cursor keys
    _MEDIA,     // Layer 1: Media controls
    _MOUSE      // Layer 2: Mouse movement
};

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    /* Base Layer: Cursor keys
     * ┌───┬───┬───┬───┐
     * │LFT│UP │DWN│RGT│
     * └───┴───┴───┴───┘ 
     */
    [_CURSOR] = LAYOUT_1x4(
        KC_LEFT, KC_UP, KC_DOWN, KC_RIGHT //QK_BOOT
     ),

    [_MEDIA] = LAYOUT_1x4(
        KC_MRWD,    // Rewind
        TO(_CURSOR), // switch to base layer
        KC_MPLY,    // Volume down
        KC_MFFD    // Fast Forward
    )
};
 
#define COMBO_TERM 150  // Time window in milliseconds for the combo

static bool left_pressed = false;
static bool right_pressed = false;
static uint16_t left_timer = 0;
static uint16_t right_timer = 0;

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    uint8_t current_layer = get_highest_layer(layer_state);
    
    switch (keycode) {
        case KC_LEFT:
            if (record->event.pressed) {
                left_pressed = true;
                left_timer = timer_read();
                if (right_pressed && current_layer != _MEDIA) {
                    // Check if RIGHT was pressed recently
                    if (timer_elapsed(right_timer) < COMBO_TERM) {
                        layer_on(_MEDIA);
                        return false;
                    }
                }
            } else {
                left_pressed = false;
            }
            break;
            
        case KC_RIGHT:
            if (record->event.pressed) {
                right_pressed = true;
                right_timer = timer_read();
                if (left_pressed && current_layer != _MEDIA) {
                    // Check if LEFT was pressed recently
                    if (timer_elapsed(left_timer) < COMBO_TERM) {
                        layer_on(_MEDIA);
                        return false;
                    }
                }
            } else {
                right_pressed = false;
            }
            break;

        default:
            if (record->event.pressed) {
                left_pressed = false;
                right_pressed = false;
            }
            break;
    }
    return true;
}
//         uprintf("Firmware built: %s\n", OMSRS32H_BUILDDATE);
