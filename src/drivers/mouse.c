#include "mouse.h"
#include "arch/interrupts.h"
#include "graphics.h"
#include "keyboard.h"
#include <stdint.h>
#include <stdlib.h>

static MouseState g_mouse_state = {.x = 0,
                                   .y = 0,
                                   .left_button = false,
                                   .middle_button = false,
                                   .right_button = false};

static uint8_t g_mouse_cycle = 0;
static uint8_t g_mouse_packet[3];
// y_overflow|x_overflow|y_sign|x_sign|1|middle_button|right_button|left_button
// x_delta
// y_delta

// For 1st packet:
#define MOUSE_PACKET_Y_OVERFLOW 0b10000000
#define MOUSE_PACKET_X_OVERFLOW 0b01000000
#define MOUSE_PACKET_Y_SIGN 0b00100000
#define MOUSE_PACKET_X_SIGN 0b00010000
#define MOUSE_PACKET_MIDDLE_BUTTON 0b00000100
#define MOUSE_PACKET_RIGHT_BUTTON 0b00000010
#define MOUSE_PACKET_LEFT_BUTTON 0b00000001

#define ENABLE_IRQ12 0b00000010
#define DISABLE_MOUSE_CLOCK 0b00100000
void enable_mouse(void) {
  ps2_flush_output_buffer();

  if (!send_command_to_keyboard_controller(ENABLE_MOUSE_PORT))
    return;
  if (!send_command_to_keyboard_controller(READ_CONTROLLER_CONFIGURATION_BYTE))
    return;

  uint8_t status = read_keyboard_data();

  if (!send_command_to_keyboard_controller(WRITE_CONTROLLER_CONFIGURATION_BYTE))
    return;
  if (!send_command_to_keyboard_encoder((status | ENABLE_IRQ12) &
                                        ~DISABLE_MOUSE_CLOCK))
    return;

  send_command_directly_to_mouse_device(SET_MOUSE_PARAMETERS_TO_DEFAULT);
  send_command_directly_to_mouse_device(ENABLE_MOUSE_DATA_STREAMING);
}

void disable_mouse(void) {
  send_command_to_keyboard_controller(DISABLE_MOUSE_PORT);
}

bool send_command_directly_to_mouse_device(uint8_t command) {
  return send_command_to_keyboard_controller(WRITE_NEXT_BYTE_TO_MOUSE_PORT) &&
         send_command_to_keyboard_encoder(command) &&
         read_keyboard_data() == PS2_ACKNOWLEDGEMENT;
}

void mouse_handler(void) {
  if (!(read_status_from_keyboard_controller() & MOUSE_DATA_NOT_KEYBOARD)) {
    end_of_interrupt(12);
    return;
  }

  uint8_t data = read_keyboard_data();
  switch (g_mouse_cycle) {
  case 0:
    if (!(data & 0b00001000)) {
      // Misaligned byte
      break;
    }
    g_mouse_cycle++;
    g_mouse_packet[0] = data;
    break;
  case 1:
    g_mouse_cycle++;
    g_mouse_packet[1] = data;
    break;
  case 2:
    g_mouse_cycle = 0;
    g_mouse_packet[2] = data;

    int16_t x_delta = g_mouse_packet[1];
    int16_t y_delta = g_mouse_packet[2];

    g_mouse_state.left_button = g_mouse_packet[0] & MOUSE_PACKET_LEFT_BUTTON;
    g_mouse_state.right_button = g_mouse_packet[0] & MOUSE_PACKET_RIGHT_BUTTON;
    g_mouse_state.middle_button =
        g_mouse_packet[0] & MOUSE_PACKET_MIDDLE_BUTTON;

    if (g_mouse_packet[0] & MOUSE_PACKET_X_SIGN) {
      x_delta |= 0xFF00;
    }

    if (g_mouse_packet[0] & MOUSE_PACKET_Y_SIGN) {
      y_delta |= 0xFF00;
    }

    if (g_mouse_packet[0] & MOUSE_PACKET_X_OVERFLOW) {
      x_delta = (g_mouse_packet[0] & MOUSE_PACKET_X_SIGN) ? INT8_MIN : INT8_MAX;
    }

    if (g_mouse_packet[0] & MOUSE_PACKET_Y_OVERFLOW) {
      y_delta = (g_mouse_packet[0] & MOUSE_PACKET_Y_SIGN) ? INT8_MIN : INT8_MAX;
    }

    const GraphicsState *graphics_state = get_graphics_state();

    if (graphics_state->width > 0 && graphics_state->height > 0) {
      g_mouse_state.x = clamp((int16_t)g_mouse_state.x + x_delta, 0,
                              graphics_state->width - 1);
      g_mouse_state.y = clamp((int16_t)g_mouse_state.y - y_delta, 0,
                              graphics_state->height - 1);
    }

    break;
  }
  end_of_interrupt(12);
}

const MouseState *get_mouse_state(void) { return &g_mouse_state; }
