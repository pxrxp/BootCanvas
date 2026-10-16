#include "interrupts.h"
#include "io.h"
#include <stdbool.h>
#include <stdint.h>

static uint8_t last_scancode;

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

uint8_t read_status_from_keyboard_controller() {
  return inb(KEYBOARD_CONTROLLER_INPUT_BUFFER);
}

void send_command_to_keyboard_controller(uint8_t command) {
  while (read_status_from_keyboard_controller() & INPUT_BUFFER_FULL)
    ;
  outb(KEYBOARD_CONTROLLER_INPUT_BUFFER, command);
}

uint8_t read_keyboard_encoder_input_buffer() {
  return inb(KEYBOARD_ENCODER_INPUT_BUFFER);
}

void send_command_to_keyboard_encoder(uint8_t command) {
  while (read_status_from_keyboard_controller() & INPUT_BUFFER_FULL)
    ; // Because keyboard controller is the one forwarding commands to keyboard
      // encoder
  outb(KEYBOARD_ENCODER_INPUT_BUFFER, command);
}

// Keyboard encoder commands
#define SET_LEDS 0xED

// Keyboard controller commands
#define ENABLE_KEYBOARD_CONTROLLER 0xAE
#define DISABLE_KEYBOARD_CONTROLLER 0xAD
#define SELF_TEST 0xAA

#define SCROLL_LOCK 0b00000001
#define NUMS_LOCK 0b00000010
#define CAPS_LOCK 0b00000100

void set_leds(bool scroll, bool nums, bool caps) {
  uint8_t command = 0;

  // !anything falsy = 1
  // !1 = 0
  // -0 = 0
  // 0 & LOCK = 0
  //
  // but
  // !1 = 0
  // !0 = 1
  // -1 = 11111111
  // 11111111 & LOCK = LOCK
  //
  // similar to if(scroll) command |= SCROLL_LOCK but efficient

  command |= (-((uint8_t)!!scroll)) & SCROLL_LOCK;
  command |= (-((uint8_t)!!nums)) & NUMS_LOCK;
  command |= (-((uint8_t)!!caps)) & CAPS_LOCK;

  send_command_to_keyboard_encoder(SET_LEDS);
  send_command_to_keyboard_encoder(command);
}

bool self_test() {
  send_command_to_keyboard_controller(SELF_TEST);
  while (!(read_status_from_keyboard_controller() & OUTPUT_BUFFER_FULL))
    ;
  return read_keyboard_encoder_input_buffer() == 0x55;
}

void enable() {
  send_command_to_keyboard_controller(ENABLE_KEYBOARD_CONTROLLER);
}

void disable() {
  send_command_to_keyboard_controller(DISABLE_KEYBOARD_CONTROLLER);
}

uint8_t read_keyboard_data(void) {
  while (!(read_status_from_keyboard_controller() & OUTPUT_BUFFER_FULL))
    ;
  return inb(KEYBOARD_ENCODER_INPUT_BUFFER);
}

void keyboard_handler(void) {
  uint8_t scancode = read_keyboard_data();
  end_of_interrupt(0x1);
}
