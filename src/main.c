#include "graphics.h"
#include "string.h"
#include "util.h"
#include <stdint.h>

extern volatile int mouse_x, mouse_y;
extern volatile uint8_t last_scancode;
extern volatile uint8_t mouse_buttons;

extern void setup_interrupts(void);

void kmain(uint32_t framebuffer, uint32_t width, uint32_t height, uint32_t bpp,
           uint32_t pitch) {
  init_graphics(framebuffer, width, height, bpp, pitch);
  setup_interrupts();

  while (1) {
    int x = mouse_x;
    int y = mouse_y;

  }
}
