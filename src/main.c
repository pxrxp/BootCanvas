#include <stdint.h>
#include "graphics.h"

void kmain(uint32_t framebuffer, uint32_t width, uint32_t height, uint32_t bpp, uint32_t pitch) {
    init_graphics(framebuffer, width, height, bpp, pitch);

    for (int i = 100; i <= 170; i++) {
        for (int j= 100; j <= 170; j++) {
            put_pixel(i, j, 255, 0, 0);
        }
    }
   
    while(1);
}
