#include "arch/io.h"
#include "drivers/graphics.h"
#include "drivers/keyboard.h"
#include "drivers/mouse.h"
#include <stdbool.h>
#include <stdint.h>

extern void setup_interrupts(void);

void kmain(uint32_t framebuffer, uint32_t width, uint32_t height, uint32_t bpp,
           uint32_t pitch) {
  init_graphics(framebuffer, width, height, bpp, pitch);
  setup_interrupts();

  volatile const KeyboardState *keyboard_state = get_keyboard_state();
  volatile const MouseState *mouse_state = get_mouse_state();

  KeyboardState last_keyboard_state = *keyboard_state;
  MouseState last_mouse_state = *mouse_state;

  while (1) {
    wait_for_interrupts();

    bool keyboard_changed =
        (keyboard_state->key_pressed != last_keyboard_state.key_pressed ||
         keyboard_state->last_scancode != last_keyboard_state.last_scancode);

    bool mouse_changed =
        (mouse_state->x != last_mouse_state.x ||
         mouse_state->y != last_mouse_state.y ||
         mouse_state->left_button != last_mouse_state.left_button ||
         mouse_state->middle_button != last_mouse_state.middle_button ||
         mouse_state->right_button != last_mouse_state.right_button);

    if (mouse_changed) {
      plot_pixel((Point){mouse_state->x, mouse_state->y},
                 (Color){255, 255, 255});
    }

    last_keyboard_state = *keyboard_state;
    last_mouse_state = *mouse_state;
  }
}
