#include "graphics.h"

void init_graphics(uint32_t framebuffer, uint32_t width, uint32_t height,
                   uint32_t bpp, uint32_t pitch) {
  g_framebuffer = (uint32_t *)framebuffer;
  g_width = width;
  g_height = height;
  g_bpp = bpp;
  g_pitch = pitch;
}

void put_pixel(int x, int y, uint8_t r, uint8_t g, uint8_t b) {
  if (x < 0 || x >= g_width || y < 0 || y >= g_height)
    return;

  uint32_t offset = y * g_pitch + x * (g_bpp / 8);
  uint8_t *fb = (uint8_t *)g_framebuffer;

  fb[offset + 0] = b;
  fb[offset + 1] = g;
  fb[offset + 2] = r;
}
