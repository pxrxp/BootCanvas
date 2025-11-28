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

void rectangle(Point top_left, Point bottom_right, Color stroke_color) {
  Point top_right = {bottom_right.x, top_left.y};
  Point bottom_left = {top_left.x, bottom_right.y};
  line(top_left, top_right, stroke_color);
  line(top_left, bottom_left, stroke_color);
  line(bottom_left, bottom_right, stroke_color);
  line(bottom_right, top_right, stroke_color);
}

void line(Point p0, Point p1, Color c) {
  int x0 = p0.x;
  int y0 = p0.y;
  int x1 = p1.x;
  int y1 = p1.y;

  int dx = (x1 > x0) ? (x1 - x0) : (x0 - x1);
  int sx = (x0 < x1) ? 1 : -1;

  int dy = (y1 > y0) ? (y1 - y0) : (y0 - y1);
  int sy = (y0 < y1) ? 1 : -1;

  int err = dx - dy;

  while (1) {
    Point p = {x0, y0};
    plot_pixel(p, c);

    if (x0 == x1 && y0 == y1)
      break;

    int e2 = err << 1;

    if (e2 > -dy) {
      err -= dy;
      x0 += sx;
    }

    if (e2 < dx) {
      err += dx;
      y0 += sy;
    }
  }
}
