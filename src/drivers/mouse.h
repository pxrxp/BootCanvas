#ifndef MOUSE_H
#define MOUSE_H

#include <stdint.h>
#include <stdbool.h>

typedef struct {
  int32_t x;
  int32_t y;
  bool left_button;
  bool middle_button;
  bool right_button;
} MouseState;

// Keyboard and mouse share the same ports 0x60, 0x64.
// We use special commands to send commands to the mouse (auxiliary) port
// instead of keyboard.

// Mouse controller commands
#define ENABLE_MOUSE_PORT 0xA8
#define DISABLE_MOUSE_PORT 0xA7
#define WRITE_NEXT_BYTE_TO_MOUSE_PORT 0xD4

// Mouse device commands
#define SET_MOUSE_PARAMETERS_TO_DEFAULT 0xF6
#define ENABLE_MOUSE_DATA_STREAMING 0xF4
#define RESET_MOUSE 0xFF

void enable_mouse(void);
void disable_mouse(void);
bool send_command_directly_to_mouse_device(uint8_t command);

const MouseState *get_mouse_state(void);

#endif
