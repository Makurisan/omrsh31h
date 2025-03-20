/*
This is the c configuration file for the keymap

Copyright 2012 Jun Wako <wakojun@gmail.com>
Copyright 2015 Jack Humbert

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#pragma once

/* key matrix size */
//#define MATRIX_ROWS 1
//#define MATRIX_COLS 4

// Enable RESET command support
//#define ENABLE_RESET_SUPPORT

#define CONSOLE_ENABLE_ECHO    // Enable echo back
#define CONSOLE_TX_BUFFER_SIZE 64
#define CONSOLE_RX_BUFFER_SIZE 64


#define MSGPACK_UNKNOWN             0
#define MSGPACK_CURRENT_KEYCODE     1
#define MSGPACK_CURRENT_LAYER       2
#define MSGPACK_CURRENT_LEDSTATE    3

//#define QUICK_TAP_TERM 0
#define TAPPING_TERM 175  // Time window for double tap (in ms)

#define RAW_EPSIZE 64  // Override the default 32 byte size

// Add build timestamp
#define OMSRS32H_BUILDDATE (__DATE__ " " __TIME__)
