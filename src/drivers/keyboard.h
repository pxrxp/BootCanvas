#ifndef KEYBOARD_H
#define KEYBOARD_H

#include "scancode.h"
#include <stdbool.h>
#include <stdint.h>

typedef struct {
  ScanCode last_scancode;
  bool key_pressed;
} KeyboardState;

#define OUTPUT_BUFFER_FULL 0b00000001
#define INPUT_BUFFER_FULL 0b00000010
#define KEYBOARD_PASSED_SELF_TEST 0b00000100
#define LAST_INPUT_WAS_A_COMMAND_NOT_DATA 0b00001000
#define KEYBOARD_LOCKED 0b00010000
#define MOUSE_DATA_NOT_KEYBOARD 0b00100000
#define TIMEOUT 0b01000000
#define PARITY_ERROR_WITH_LAST_BYTE 0b10000000

// Same port to read and send command
// Keyboard encoder is in the keyboard itself
#define KEYBOARD_ENCODER_INPUT_BUFFER 0x60

// Same port to read and send command
// Keyboard controller is on the motherboard
#define KEYBOARD_CONTROLLER_INPUT_BUFFER 0x64

uint8_t read_status_from_keyboard_controller();
bool send_command_to_keyboard_controller(uint8_t command);
uint8_t read_keyboard_encoder_input_buffer();
bool send_command_to_keyboard_encoder(uint8_t command);

bool wait_for_input_buffer_to_clear(void);
bool wait_for_output_buffer_to_fill(void);

// Keyboard encoder commands
#define SET_LEDS 0xED
#define START_SCANNING_KEYPRESSES 0xF4

// Keyboard controller commands
#define ENABLE_KEYBOARD_PORT 0xAE
#define DISABLE_KEYBOARD_PORT 0xAD
#define SELF_TEST 0xAA
#define READ_CONTROLLER_CONFIGURATION_BYTE 0x20
#define WRITE_CONTROLLER_CONFIGURATION_BYTE 0x60

// Returned by PS/2 device
#define PS2_ACKNOWLEDGEMENT 0xFA

#define SCROLL_LOCK 0b00000001
#define NUMS_LOCK 0b00000010
#define CAPS_LOCK 0b00000100

void set_leds(bool scroll, bool nums, bool caps);
bool self_test_keyboard(void);
void enable_keyboard(void);
void disable_keyboard(void);
uint8_t read_keyboard_data(void);

void ps2_flush_output_buffer(void);
void keyboard_handler(void);

const KeyboardState *get_keyboard_state(void);

#endif
