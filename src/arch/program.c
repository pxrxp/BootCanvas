#include "arch/program.h"
#include "drivers/keyboard.h"
#include "drivers/mouse.h"
#include "stdlib.h"

static Program current_program;
volatile const KeyboardState *keyboard_state;
volatile const MouseState *mouse_state;
static bool initialized;

KeyboardState last_keyboard_state;
MouseState last_mouse_state;

void set_current_program(Program p) { current_program = p; }

void init_program() {
  current_program.init = NULL;
  current_program.run = NULL;
  initialized = false;

  keyboard_state = get_keyboard_state();
  mouse_state = get_mouse_state();

  last_keyboard_state = *keyboard_state;
  last_mouse_state = *mouse_state;
}

void run_program() {
  keyboard_state = get_keyboard_state();
  mouse_state = get_mouse_state();

  bool keyboard_changed =
      (keyboard_state->key_pressed != last_keyboard_state.key_pressed ||
       keyboard_state->last_scancode != last_keyboard_state.last_scancode);

  bool mouse_changed =
      (mouse_state->x != last_mouse_state.x ||
       mouse_state->y != last_mouse_state.y ||
       mouse_state->left_button != last_mouse_state.left_button ||
       mouse_state->middle_button != last_mouse_state.middle_button ||
       mouse_state->right_button != last_mouse_state.right_button);

  last_keyboard_state = *keyboard_state;
  last_mouse_state = *mouse_state;

  if (!initialized && current_program.init) {
    current_program.init();
    initialized = true;
  }

  if (current_program.run)
    current_program.run(keyboard_changed, mouse_changed);
}
