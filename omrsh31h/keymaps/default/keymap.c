
#include QMK_KEYBOARD_H

#include "config.h"

// Combined enum for all custom keycodes
enum custom_keycodes {
    CC_SS = SAFE_RANGE,
    CC_DB
};

enum layers {
    _BASE,    // Layer 0: Cursor keys
    _MEDIA,     // Layer 1: Media controls
    _YOUTUBE    // Layer 2: YouTube controls
};

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {

    [_BASE] = LAYOUT_1x4(
        KC_LEFT, KC_UP, KC_DOWN, KC_RIGHT 
     ),

    [_MEDIA] = LAYOUT_1x4(
        KC_MRWD,    // Rewind
        KC_MUTE,    // Sound mute toggle
        KC_MPLY,    // Volume down
        KC_MFFD     // Fast Forward
    ),
    [_YOUTUBE] = LAYOUT_1x4(
        KC_LEFT, QK_BOOT, KC_SPACE, KC_RIGHT 
     ),
};
 
#define COMBO_TERM 250  // Time window in milliseconds for the combo

static bool left_pressed = false;
static bool right_pressed = false;
static uint16_t left_timer = 0;
static uint16_t right_timer = 0;

#define PHYSICAL_LEFT 0
#define PHYSICAL_RIGHT 3

#define LAYER_SWITCH_PIN GP8  
#define RESET_SWITCH_PIN GP7  
#define LED_SWITCH_PIN GP6  

 #define CYCLE_LAYER(current) ((current) < _YOUTUBE? (current) + 1 : _BASE)

void baselayer_led_on(void) {
    gpio_set_pin_output(LED_SWITCH_PIN);
    gpio_write_pin_high(LED_SWITCH_PIN);
}

void baselayer_led_off(void) {
    gpio_set_pin_input(LED_SWITCH_PIN);
   //gpio_write_pin_low(LED_SWITCH_PIN);
}

bool process_record_user(uint16_t keycode, keyrecord_t *record) {

    uint8_t col = record->event.key.col;
    uint8_t current_layer = get_highest_layer(layer_state);

    if (!readPin(RESET_SWITCH_PIN)) {
        // Foot switch pressed - send your desired keycode
        uprintf("EXTRAKEY: Layer before=%u\n", current_layer);
        bootloader_jump();
        return false;
    }
    if (!readPin(LAYER_SWITCH_PIN)) {
        layer_move(_BASE);
        return false;
    }
    //uprintf("Left/Right: %u %u %u\n", left_pressed, right_pressed, current_layer);   

     switch (col) {

        case PHYSICAL_LEFT:
            if (record->event.pressed) {
                left_pressed = true;
                left_timer = timer_read();
                if (right_pressed && timer_elapsed(right_timer) < COMBO_TERM) {
                    //uprintf("LEFT: Layer before=%u\n", current_layer);
                    layer_move(CYCLE_LAYER(current_layer));
                    return false;
                }
            } else {
                left_pressed = false;
            }
            break;
            
        case PHYSICAL_RIGHT:
            if (record->event.pressed) {
                right_pressed = true;
                right_timer = timer_read();
                if (left_pressed && timer_elapsed(left_timer) < COMBO_TERM) {
                    //uprintf("RIGHT: Layer before=%u\n", current_layer);
                    layer_move(CYCLE_LAYER(current_layer));
                    return false;
                }
            } else {
                right_pressed = false;
            }
            break;
 
        default:

            break;
    }
    return true;
}
//         uprintf("Firmware built: %s\n", OMSRS32H_BUILDDATE);
void matrix_scan_user(void) {
    static uint8_t old_layer = 255;
    uint8_t current = get_highest_layer(layer_state);
    
    if (old_layer != current) {
        old_layer = current;

        if (current == _BASE) {
            baselayer_led_on();
            //uprintf("Layer: %u pin: %d \n", current, (int)1);
        } else{
            baselayer_led_off();
            //uprintf("Layer: %u pin: %d \n", current, (int)0);
        }
        uprintf("{\"layer\": %d}", current);
    }
}

void keyboard_post_init_user(void) {

    // Set the pin as input with pull-up
    setPinInputHigh(RESET_SWITCH_PIN);
    setPinInputHigh(LAYER_SWITCH_PIN);

    baselayer_led_on();

    // Customise these values to desired behaviour
    //debug_matrix=true;
    //debug_keyboard=true;
    //debug_mouse=true;


   }