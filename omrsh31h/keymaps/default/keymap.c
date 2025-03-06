#include QMK_KEYBOARD_H

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
     * │LFT│DWN│ UP│RGT│
     * └───┴───┴───┴───┘ 
     */
    [_CURSOR] = LAYOUT_1x4(
        KC_A, KC_S, KC_D, KC_F
    ),

    [1] = LAYOUT_1x4(
        KC_TRNS,     // Keep layer toggle
        KC_VOLD,     // Volume down
        KC_VOLU,     // Volume up
        KC_MPLY      // Play/Pause
    )
};
