#include "graphics.h"
#include <stdint.h>

void kmain(uint32_t framebuffer, uint32_t width, uint32_t height, uint32_t bpp,
           uint32_t pitch) {
  init_graphics(framebuffer, width, height, bpp, pitch);

  rectangle((Point){1,1}, (Point){width-1, height-1}, 0, (Color){255,255,255});

  ellipse((Point){width/2, height/2}, 150, 300, 15, (Color){255,255, 0});

  while (1)
    ;
}
