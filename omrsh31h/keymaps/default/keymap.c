
#include QMK_KEYBOARD_H
#include "config.h"
#include "raw_hid.h"
#include "../msgpack.h"


#ifdef VIRT_SER_ENABLED
#include "virtser.h"
#endif

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

#define LAYER_COUNT sizeof(keymaps) / sizeof(keymaps[0])

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

 #define CYCLE_LAYER(current) ((current) < (LAYER_COUNT-1)? (current) + 1 : 0)

#ifdef VIRT_SER_ENABLED
void virtser_send_str(const char* str);
#endif

void baselayer_led_on(void) {
    gpio_set_pin_output(LED_SWITCH_PIN);
    gpio_write_pin_high(LED_SWITCH_PIN);
}

void baselayer_led_off(void) {
    gpio_set_pin_input(LED_SWITCH_PIN);
   //gpio_write_pin_low(LED_SWITCH_PIN);
}

void raw_hid_receive(uint8_t *data, uint8_t length) {

    uprintf("HID receive - length: %d\n", length);

    // Check if we received the expected packet size
    if (length != RAW_EPSIZE) {
        return;
    }

    // Example: Parse incoming JSON-like commands
    msgpack_t km={0};
    if (msgpack_read(&km, (char*)data, length)){
        // Process JSON command
        msgpack_log(&km);
    }
}

bool process_record_user(uint16_t keycode, keyrecord_t *record) {

    uint8_t col = record->event.key.col;
    uint8_t current_layer = get_highest_layer(layer_state);

    uprintf("process_record_user - keycode: %d\n", keycode);

    if (record->event.pressed ) {
        msgpack_t msgpack_pairs;
        msgpack_init(&msgpack_pairs);
        msgpack_add(&msgpack_pairs, MSGPACK_CURRENT_KEYCODE, keycode);
        msgpack_send(&msgpack_pairs);
    }

    if (!readPin(RESET_SWITCH_PIN)) {
        // Foot switch pressed - send your desired keycode
        //uprintf("EXTRAKEY: Layer before=%u\n", current_layer);
        //bootloader_jump();
        layer_move(CYCLE_LAYER(current_layer));
        return false;
    }
    if (!readPin(LAYER_SWITCH_PIN)) {
        //layer_move(_BASE);
         layer_move(CYCLE_LAYER(current_layer));
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

#ifdef VIRT_SER_ENABLED
// Handle incoming serial data
void virtser_recv(uint8_t byte) {
    // Echo back received byte
    virtser_send(byte);

    // Print debug info
    uprintf("Received byte: 0x%02X\n", byte);
}
#endif

//         uprintf("Firmware built: %s\n", OMSRS32H_BUILDDATE);
void matrix_scan_user(void) {
    static uint8_t old_layer = 255;
    uint8_t current = get_highest_layer(layer_state);

    if (old_layer != current) {
        old_layer = current;

        msgpack_t msgpack_pairs;
        msgpack_init(&msgpack_pairs);

        uint8_t led_state = 0;
        if (current == _BASE) {
            baselayer_led_on();
            led_state = 1;
        } else{
            baselayer_led_off();
            led_state = 0;
        }
        msgpack_add(&msgpack_pairs, MSGPACK_CURRENT_LEDSTATE, led_state);
        msgpack_add(&msgpack_pairs, MSGPACK_CURRENT_LAYER, current);
        msgpack_send(&msgpack_pairs);
    }
}

#ifdef VIRT_SER_ENABLED
// Helper function to send strings over virtual serial
void virtser_send_str(const char* str) {
    if (!str) return;

    while (*str) {
        virtser_send((uint8_t)*str++);
    }
}
#endif

void keyboard_post_init_user(void) {

    // Set the pin as input with pull-up
    setPinInputHigh(RESET_SWITCH_PIN);
    setPinInputHigh(LAYER_SWITCH_PIN);

    baselayer_led_on();

    #ifdef VIRT_SER_ENABLED
        virtser_init();
        char data[100] = {0};
        sprintf(data, "{\"layer\": %d}", 0);
        virtser_send_str("Keyboard initialized!\n");
    #endif

    // Customise these values to desired behaviour
    //debug_matrix=true;
    //debug_keyboard=true;
    //debug_mouse=true;


   }
