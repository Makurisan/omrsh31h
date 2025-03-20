/* Copyright 2021 Seth Bonner <fl3tching101@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

 #pragma once

 #include <stdint.h>


 #define MSGPACK_PAIR_ARRAY_SIZE 10

// Define data structure
typedef struct {
    uint8_t key;
    uint8_t value;
} msgpack_pair_t;

typedef struct {
    uint8_t count;
    msgpack_pair_t pairs[MSGPACK_PAIR_ARRAY_SIZE];
} msgpack_t;

bool msgpack_add(msgpack_t *msgpack, uint8_t key, uint8_t value);
void msgpack_init(msgpack_t *msgpack);
void msgpack_send(msgpack_t *msgpack);
bool msgpack_read(msgpack_t * km, char *buffer, size_t length);
bool msgpack_log(msgpack_t* km);
