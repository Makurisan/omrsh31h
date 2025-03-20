#include QMK_KEYBOARD_H
#include "config.h"
#include "raw_hid.h"
#include "msgpack.h"
#include "mpack.h"


typedef struct {
    uint8_t key;
    const char *name;
} msgpack_key_t;

static msgpack_key_t msgpack_keys[] = {
    {MSGPACK_UNKNOWN, "unknown"},
    {MSGPACK_CURRENT_KEYCODE, "keycode"},
    {MSGPACK_CURRENT_LAYER, "layer"},
    {MSGPACK_CURRENT_LEDSTATE, "ledstate"}
};

void mpack_assert_fail(const char* message) {
    uprintf("MessagePack assertion failed: %s\n", message);
    while(1) {} // Halt on assertion failure
}

// Implementation
void msgpack_init(msgpack_t * km) {
    km->count = 0;
    // Initialize all pairs to 0
    memset(km->pairs, 0, sizeof(msgpack_pair_t) * MSGPACK_PAIR_ARRAY_SIZE);
}

// Helper function to add a pair
bool msgpack_add(msgpack_t * km, uint8_t key, uint8_t value) {
    if (km->count >= 10) return false;  // Array full

    km->pairs[km->count].key = key;
    km->pairs[km->count].value = value;
    km->count++;
    return true;
}

void msgpack_send(msgpack_t * km) {
    char buffer[RAW_EPSIZE];
    mpack_writer_t writer;

    mpack_writer_init(&writer, buffer, sizeof(buffer));

    // Write format identifier string "MPACK"
    mpack_write_cstr(&writer, "QMV1");

    // Start writing map with number of pairs
    mpack_start_map(&writer, km->count);
    // Loop through all pairs
    for (size_t i = 0; i < km->count; i++) {
        mpack_write_uint(&writer, km->pairs[i].key);
        mpack_write_uint(&writer, km->pairs[i].value);
    }

    mpack_finish_map(&writer);

    if (mpack_writer_destroy(&writer) == mpack_ok) {
        raw_hid_send((uint8_t*)buffer, RAW_EPSIZE);
        uprintf("Sent %d key-value pairs\n", km->count);
    }
}

bool msgpack_read(msgpack_t * km, char * buffer, size_t length) {
     mpack_reader_t reader;
    bool success = false;

    mpack_reader_init_data(&reader, buffer, length);

    // Check format identifier
    char format[5];
    mpack_expect_cstr(&reader, format, sizeof(format));
    if (strcmp(format, "QMV1") != 0) {
        uprintf("Invalid format identifier\n");
        goto cleanup;
    }

    // Read map
    uint32_t count = mpack_expect_map(&reader);
    if (count > MSGPACK_PAIR_ARRAY_SIZE) {
        uprintf("Too many pairs received: %lu\n", count);
        goto cleanup;
    }

    // Initialize msgpack structure
    msgpack_init(km);

    // Read all key-value pairs
    for (uint32_t i = 0; i < count; i++) {
        uint8_t key = mpack_expect_uint(&reader);
        uint8_t value = mpack_expect_uint(&reader);
        msgpack_add(km, key, value);
        uprintf("key %s key-value: %d\n", msgpack_keys[key].name, value);
    }
    mpack_done_map(&reader);
    success = true;
    uprintf("Received %d key-value pairs\n", km->count);

cleanup:
    if (mpack_reader_destroy(&reader) != mpack_ok) {
        uprintf("Error reading msgpack data\n");
        return false;
    }
    return success;
}

bool msgpack_log(msgpack_t* km) {
    char outmsg[1024] = ""; // Buffer to hold the output message
    char buffer[100]; // Temporary buffer for each key-value pair

    for (uint32_t i = 0; i < km->count; i++) {
        // Format the key-value pair into the buffer
        sprintf(buffer, "Key: %u, Value: %u\n", km->pairs[i].key, km->pairs[i].value);
        // Append the buffer to the output message
        strcat(outmsg, buffer);
    }

    // Output the message using OutputDebugString
    uprintf(outmsg);

    return true;
}
