#include "graphics.h"
#include "string.h"
#include "util.h"
#include <stdint.h>

extern volatile int mouse_x, mouse_y;
extern volatile uint8_t last_scancode;
extern volatile uint8_t mouse_buttons;

extern volatile uint32_t mouse_irq_count;
extern void setup_interrupts(void);

void kmain(uint32_t framebuffer, uint32_t width, uint32_t height, uint32_t bpp,
           uint32_t pitch) {
  init_graphics(framebuffer, width, height, bpp, pitch);
  setup_interrupts();

  rectangle((Point){0, 0}, (Point){width - 1, height - 1}, 0,
            (Color){255, 255, 255});
  ellipse((Point){width / 2, height / 2}, 150, 300, 0, (Color){255, 255, 0});

  char str[6];

  while (1) {
    int x = mouse_x;
    int y = mouse_y;

    if (x >= 0 && x < (int)width && y >= 0 && y < (int)height) {
      plot_pixel((Point){x, y}, (Color){255, 255, 255});
    }
  }
}
