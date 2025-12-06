#include "graphics.h"
#include "util.h"
#include <stdint.h>

// extern void irq_init(void);
// extern volatile int mouse_x, mouse_y;
// extern volatile uint8_t last_scancode;
// extern volatile uint8_t mouse_buttons;

void kmain(uint32_t framebuffer, uint32_t width, uint32_t height, uint32_t bpp,
           uint32_t pitch) {
  init_graphics(framebuffer, width, height, bpp, pitch);

  rectangle((Point){0, 0}, (Point){width - 1, height - 1}, 0,
            (Color){255, 255, 255});
  ellipse((Point){width / 2, height / 2}, 150, 300, 0, (Color){255, 255, 0});

  while(1);
}
