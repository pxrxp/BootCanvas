#include "graphics.h"
#include <stdint.h>

void kmain(uint32_t framebuffer, uint32_t width, uint32_t height, uint32_t bpp,
           uint32_t pitch) {
  init_graphics(framebuffer, width, height, bpp, pitch);

  rectangle((Point){10,20}, (Point){40,60}, (Color){255,255,255});

  while (1)
    ;
}
