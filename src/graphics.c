#include "graphics.h"

void init_graphics(uint32_t framebuffer, uint32_t width, uint32_t height,
                   uint32_t bpp, uint32_t pitch) {
  g_framebuffer = (uint32_t *)framebuffer;
  g_width = width;
  g_height = height;
  g_bpp = bpp;
  g_pitch = pitch;
}

void plot_pixel(Point p, Color color) {
  if (p.x < 0 || p.x >= g_width || p.y < 0 || p.y >= g_height)
    return;

  uint32_t offset = p.y * g_pitch + p.x * (g_bpp / 8);
  uint8_t *fb = (uint8_t *)g_framebuffer;

  fb[offset + 0] = color.b;
  fb[offset + 1] = color.g;
  fb[offset + 2] = color.r;
}

void rectangle(Point top_left, Point bottom_right, Color fill_color,
               Color stroke_color) {
  for (int x = top_left.x; x <= bottom_right.x; x++) {
    for (int y = top_left.y; y <= bottom_right.y; y++) {
      plot_pixel((Point){x, y}, (x == top_left.x || y == top_left.y ||
                                 x == bottom_right.x || y == bottom_right.y)
                                    ? stroke_color
                                    : fill_color);
    }
  }
}
