#include <stdint.h>
#include "graphics.h"

void kmain(uint32_t framebuffer, uint32_t width, uint32_t height, uint32_t bpp, uint32_t pitch) {
    init_graphics(framebuffer, width, height, bpp, pitch);

    for (int i = 0; i < width; i++) {
        for (int j = 0; j < height; j++) {
            plot_pixel((Point){ i, j}, (Color){255,0,0});
        }
    }
  
    while(1);
}
