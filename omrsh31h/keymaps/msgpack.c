#include QMK_KEYBOARD_H
#include "config.h"
#include "raw_hid.h"
#include "msgpack.h"

#define MAX_ENTRIES 16

typedef struct {
    uint8_t key;
    const char *name;
} msgpack_key_t;

static msgpack_key_t msgpack_keys[] = {
    {MSGPACK_UNKNOWN, "unknown"},
    {MSGPACK_CURRENT_KEYCODE, "keycode"},
    {MSGPACK_CURRENT_LAYER, "layer"},
    {MSGPACK_CURRENT_GETLAYER, "get_layer"},
    {MSGPACK_CURRENT_LEDSTATE, "ledstate"}
};

// Function to create a MessagePack map with variable entries
int8_t msgpack_send(const msgpack_t* msgpack) {
    if (msgpack->count > MAX_ENTRIES) {
        return -1; // Exceeds maximum allowed entries
    }

    uint8_t buffer[RAW_EPSIZE] = {0}; // Buffer to hold the output message
    uint16_t buffersize = RAW_EPSIZE; // Size of the provided buffer

      // Calculate the required buffer size
      size_t string_size = 1 + 4; // 1 byte for the string header, 4 bytes for "QMV1"
      size_t map_size = 1 + msgpack->count * 4; // 1 byte for the map header, 1 byte for key, 1 byte for int16 marker, 2 bytes for each value
      size_t required_size = string_size + map_size;

      if (buffersize < required_size) {
          return -2; // Provided buffer is too small
      }

      // Initialize the buffer
      uint8_t* ptr = buffer;

      // Write the string "QMV1" in MessagePack format
      *ptr++ = 0xa4; // FixStr format for a string with 4 characters
      memcpy(ptr, "QMV1", 4);
      ptr += 4;

      // Write the map header (fixmap for msgpack->count entries)
      *ptr++ = 0x80 | msgpack->count; // 0x80 + number of entries

      // Write the key-value pairs
      for (size_t i = 0; i < msgpack->count; ++i) {
          *ptr++ = msgpack->pairs[i].key;   // Key
          *ptr++ = 0xd1; // Int 16 marker
          *ptr++ = (msgpack->pairs[i].value >> 8) & 0xFF; // High byte of value
          *ptr++ = msgpack->pairs[i].value & 0xFF;        // Low byte of value
      }
    raw_hid_send((uint8_t*)buffer, RAW_EPSIZE);
    uprintf("Sent %d key-value pairs\n", msgpack->count);

    // Return the length of the created buffer
    return (int8_t)required_size;
}

bool msgpack_read(msgpack_t * km, char *buffer, uint8_t length) {
    if (!km || !buffer || length < 5) {
        uprintf("Error: Invalid parameters\n");
        return false;
    }

    char *ptr = buffer;
    uint8_t remaining = length;

    // Verify "QMV1" header (4 bytes + marker)
    if (remaining < 5 || *ptr != 0xa4) {
        uprintf("Error: Invalid header marker\n");
        return false;
    }
    ptr++;
    remaining--;

    if (strncmp(ptr, "QMV1", 4) != 0) {
        uprintf("Error: Invalid header string\n");
        return false;
    }
    ptr += 4;
    remaining -= 4;

    // Verify map marker and get count
    if (remaining < 1 || (*ptr & 0xf0) != 0x80) {
        uprintf("Error: Invalid map marker\n");
        return false;
    }
    uint8_t map_size = *ptr & 0x0f;
    ptr++;
    remaining--;

    // Reset message pack structure
    msgpack_init(km);

    // Read all key-value pairs
    for (uint8_t i = 0; i < map_size; i++) {
        if (remaining < 2) {  // Need at least key + value type
            uprintf("Error: Message truncated at pair %d\n", i);
            return false;
        }

        // Read key
        uint8_t key = *ptr;
        ptr++;
        remaining--;

        // Check value type
        int16_t value;
        if (*ptr == 0xd1) {  // int16 value
            ptr++;  // Skip marker
            uint8_t high = *ptr++;
            uint8_t low = *ptr++;
            value = (high << 8) | low;
            remaining -= 3;
        } else if (*ptr == 0xd0) {  // int8 value
            ptr++;  // Skip marker
            int8_t byte = *ptr++;
            value = byte;
            remaining -= 2;
        }else { // uint8 only to show
            int8_t byte = *ptr++;
            value = byte;
            remaining--;
         }

        // Add pair to message pack structure
        if (!msgpack_add(km, key, value)) {
            uprintf("Error: Failed to add pair %d\n", i);
            return false;
        }
        uprintf( "Key: %d, Value: %d\n", key, value);

    }
    uprintf("Successfully read %d pairs\n", km->count);
    return true;
}

// Helper function to add a pair
bool msgpack_add(msgpack_t * km, uint8_t key, int16_t value) {
    if (km->count >= 10) return false;  // Array full

    km->pairs[km->count].key = key;
    km->pairs[km->count].value = value;
    km->count++;
    return true;
}

void msgpack_init(msgpack_t* km) {
    memset(km, 0, sizeof(msgpack_t));
}

bool msgpack_log(msgpack_t* km) {
    char outmsg[1024] = {}; // Buffer to hold the output message
    char buffer[100]= {}; // Temporary buffer for each key-value pair

    for (uint32_t i = 0; i < km->count; i++) {
        // Format the key-value pair into the buffer
        sprintf(buffer, "Key: %s, Value: %d\n", msgpack_keys[km->pairs[i].key].name, km->pairs[i].value);
        // Append the buffer to the output message
        strcat(outmsg, buffer);
    }

    // Output the message using OutputDebugString
    uprintf(outmsg);

    return true;
}
