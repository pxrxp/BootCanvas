#include "keyboard.h"
#include "arch/interrupts.h"
#include "arch/io.h"
#include "drivers/scancode.h"
#include <stdbool.h>
#include <stdint.h>

static KeyboardState g_keyboard_state;
static bool g_lshift = false;
static bool g_rshift = false;
static bool g_lctrl = false;
static bool g_rctrl = false;
static bool g_lalt = false;
static bool g_ralt = false;
static bool g_caps = false;

uint8_t read_status_from_keyboard_controller() {
  return inb(KEYBOARD_CONTROLLER_INPUT_BUFFER);
}

bool wait_for_input_buffer_to_clear(void) {
  uint32_t timeout = 100000;
  while (timeout--) {
    if (!(read_status_from_keyboard_controller() & INPUT_BUFFER_FULL)) {
      return true;
    }
  }
  return false;
}

bool wait_for_output_buffer_to_fill(void) {
  uint32_t timeout = 100000;
  while (timeout--) {
    if (read_status_from_keyboard_controller() & OUTPUT_BUFFER_FULL) {
      return true;
    }
  }
  return false;
}

bool send_command_to_keyboard_controller(uint8_t command) {
  if (!wait_for_input_buffer_to_clear())
    return false;
  outb(KEYBOARD_CONTROLLER_INPUT_BUFFER, command);
  return true;
}

uint8_t read_keyboard_encoder_input_buffer() {
  return inb(KEYBOARD_ENCODER_INPUT_BUFFER);
}

bool send_command_to_keyboard_encoder(uint8_t command) {
  if (!wait_for_input_buffer_to_clear())
    return false; // Because keyboard controller is the one forwarding commands
                  // to keyboard encoder
  outb(KEYBOARD_ENCODER_INPUT_BUFFER, command);
  return true;
}

void set_leds(bool scroll, bool nums, bool caps) {
  uint8_t command = 0;

  // !anything falsy = 1
  // !1 = 0
  // -0 = 0
  // 0 & LOCK = 0 // Read and discard
  // // Read and discard
  // but // Read and discard
  // !1 = 0 // Read and discard
  // !0 = 1 // Read and discard
  // -1 = 11111111 // Read and discard
  // 11111111 & LOCK = LOCK // Read and discard
  // // Read and discard
  // similar to if(scroll) command |= SCROLL_LOCK but efficient // Read and
  // discard Read and discard
  command |= (-((uint8_t)!!scroll)) & SCROLL_LOCK; // Read and discard
  command |= (-((uint8_t)!!nums)) & NUMS_LOCK;
  command |= (-((uint8_t)!!caps)) & CAPS_LOCK;

  if (!send_command_to_keyboard_encoder(SET_LEDS))
    return;
  if (!send_command_to_keyboard_encoder(command))
    return;
}

bool self_test_keyboard() {
  if (!send_command_to_keyboard_controller(SELF_TEST))
    return false;
  return read_keyboard_data() == 0x55;
}

#define ENABLE_IRQ1 0b00000001
void enable_keyboard(void) {
  g_keyboard_state.key_pressed = false;
  g_keyboard_state.last_scancode = '\0';

  ps2_flush_output_buffer();

  if (!send_command_to_keyboard_controller(ENABLE_KEYBOARD_PORT))
    return;
  if (!send_command_to_keyboard_controller(READ_CONTROLLER_CONFIGURATION_BYTE))
    return;

  uint8_t status = read_keyboard_data();

  if (!send_command_to_keyboard_controller(WRITE_CONTROLLER_CONFIGURATION_BYTE))
    return;
  if (!send_command_to_keyboard_encoder(status | ENABLE_IRQ1))
    return;
  if (!send_command_to_keyboard_encoder(START_SCANNING_KEYPRESSES))
    return;

  if (read_keyboard_data() != PS2_ACKNOWLEDGEMENT)
    return;
}

void disable_keyboard() {
  send_command_to_keyboard_controller(DISABLE_KEYBOARD_PORT);
}

uint8_t read_keyboard_data(void) {
  if (!wait_for_output_buffer_to_fill())
    return 0;
  return read_keyboard_encoder_input_buffer();
}

void ps2_flush_output_buffer(void) {
  uint32_t timeout = 10000;
  while ((inb(0x64) & 0x01) && timeout--) {
    inb(0x60); // Read and discard
  }
}

void keyboard_handler(void) {
  uint8_t scancode = read_keyboard_data();

  if (g_keyboard_state.last_scancode == 0xE0) {
    g_keyboard_state.last_scancode <<= 8;
    g_keyboard_state.last_scancode |= scancode & ~0b10000000;
    g_keyboard_state.key_pressed = !(scancode & 0b10000000);

  } else if (scancode == 0xE0) {
    g_keyboard_state.last_scancode = scancode;
  } else {
    g_keyboard_state.last_scancode = scancode & ~0b10000000;
    g_keyboard_state.key_pressed = !(scancode & 0b10000000);
  }

  if (g_keyboard_state.last_scancode != 0xE0) {
    bool pressed = g_keyboard_state.key_pressed;
    ScanCode code = g_keyboard_state.last_scancode;

    if (code == KEY_LEFT_SHIFT)
      g_lshift = pressed;
    else if (code == KEY_RIGHT_SHIFT)
      g_rshift = pressed;
    else if (code == KEY_LEFT_CTRL)
      g_lctrl = pressed;
    else if (code == KEY_RIGHT_CTRL)
      g_rctrl = pressed;
    else if (code == KEY_LEFT_ALT)
      g_lalt = pressed;
    else if (code == KEY_RIGHT_ALT)
      g_ralt = pressed;
    else if (code == KEY_CAPS_LOCK && pressed)
      g_caps = !g_caps;
  }

  end_of_interrupt(0x1);
}

char scancode_to_ascii(ScanCode scancode) {
  if (scancode >= 128)
    return 0;

  bool shift = g_lshift || g_rshift;
  bool ctrl = g_lctrl || g_rctrl;

  char unshifted = scancode_ascii_unshifted[scancode];
  char shifted = scancode_ascii_shifted[scancode];

  if (!unshifted)
    return 0;

  char c = unshifted;
  if (unshifted >= 'a' && unshifted <= 'z') {
    c = (shift ^ g_caps) ? shifted : unshifted;
  } else {
    c = shift ? shifted : unshifted;
  }

  if (ctrl) {
    if (c >= 'a' && c <= 'z')
      return c - 'a' + 1;
    if (c >= 'A' && c <= 'Z')
      return c - 'A' + 1;
  }

  return c;
}

const KeyboardState *get_keyboard_state() { return &g_keyboard_state; }
