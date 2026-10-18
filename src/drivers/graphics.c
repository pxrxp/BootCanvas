#include "graphics.h"
#include "font.h"
#include <math.h>
#include <stdbool.h>

static GraphicsState g_graphics_state;

#define MAX_HEIGHT 1080
static uint32_t g_y_offsets[MAX_HEIGHT];
static uint32_t bytes_per_pixel;

static Point g_p;

void init_graphics(uint32_t framebuffer, uint32_t width, uint32_t height,
                   uint32_t bits_per_pixel, uint32_t pitch) {
  g_graphics_state.framebuffer = (uint32_t *)(uintptr_t)framebuffer;
  g_graphics_state.width = width;
  g_graphics_state.height = height;
  g_graphics_state.bits_per_pixel = bits_per_pixel;
  bytes_per_pixel = g_graphics_state.bits_per_pixel >> 3;
  g_graphics_state.pitch = pitch;

  for (int y = 0; y < g_graphics_state.height; y++) {
    g_y_offsets[y] = y * g_graphics_state.pitch;
  }
}

const GraphicsState *get_graphics_state(void) { return &g_graphics_state; }

Point rotate(Point center, Point p, int angle_deg) {
  int dx = p.x - center.x;
  int dy = p.y - center.y;
  return (Point){center.x + (dx * cos(angle_deg) - dy * sin(angle_deg)),
                 center.y + (dx * sin(angle_deg) + dy * cos(angle_deg))};
}

void plot_pixel(Point p, Color color) {
  if (p.x < 0 || p.x >= g_graphics_state.width || p.y < 0 ||
      p.y >= g_graphics_state.height)
    return;

  uint8_t *fb = (uint8_t *)g_graphics_state.framebuffer + g_y_offsets[p.y] +
                p.x * bytes_per_pixel;
  fb[0] = color.b;
  fb[1] = color.g;
  fb[2] = color.r;
}

void put_char(char c, Point *p, int start_x, Color foreground, Color background,
              unsigned int x_space, unsigned int y_space) {
  if (!p)
    return;

  filled_rectangle(*p, (Point){p->x + CHARACTER_WIDTH, p->y + CHARACTER_HEIGHT},
                   background);

  uint8_t *fb = (uint8_t *)g_graphics_state.framebuffer;

  if (c == '\n') {
    p->x = start_x;
    p->y += 8 + y_space;
    return;
  }

  if (c == '\t') {
    p->x += (8 + x_space) * 4;
    return;
  }

  if (p->x + 8 > g_graphics_state.width) {
    p->x = start_x;
    p->y += 8 + y_space;
  }
  if (p->y + 8 > g_graphics_state.height)
    return;

  if (c >= '!' && c <= '~') {
    const uint8_t *glyph = font_map[c - '!'];

    for (int y = 0; y < 8; y++) {
      uint8_t row = glyph[y];
      if (!row)
        continue;

      uint32_t row_offset =
          (p->y + y) * g_graphics_state.pitch + p->x * bytes_per_pixel;
      uint8_t *pixel_ptr = fb + row_offset;

      for (int x = 0; x < 8; x++) {
        if (row & 1) {
          pixel_ptr[0] = foreground.b;
          pixel_ptr[1] = foreground.g;
          pixel_ptr[2] = foreground.r;
        }
        pixel_ptr += bytes_per_pixel;
        row >>= 1;
      }
    }
  }

  p->x += 8 + x_space;
}

void put_text(const char *text, Point top_left, Color foreground,
              Color background, unsigned int x_space, unsigned int y_space) {
  if (!text)
    return;

  Point p = top_left;
  while (*text) {
    put_char(*text++, &p, top_left.x, foreground, background, x_space, y_space);
  }
}

void rectangle(Point top_left, Point bottom_right, int rot_angle_deg,
               Color stroke_color) {

  Point top_right = {bottom_right.x, top_left.y};
  Point bottom_left = {top_left.x, bottom_right.y};

  line(top_left, top_right, stroke_color);
  line(top_left, bottom_left, stroke_color);
  line(bottom_left, bottom_right, stroke_color);
  line(bottom_right, top_right, stroke_color);
}

void filled_rectangle(Point top_left, Point bottom_right, Color color) {
  uint8_t *fb = (uint8_t *)g_graphics_state.framebuffer;
  uint32_t pitch = g_graphics_state.pitch;

  for (uint32_t y = top_left.y; y < bottom_right.y; y++) {
    uint8_t *row = fb + (y * pitch) + (top_left.x * bytes_per_pixel);
    for (uint32_t x = top_left.x; x < bottom_right.x; x++) {
      row[0] = color.b;
      row[1] = color.g;
      row[2] = color.r;
      row += bytes_per_pixel;
    }
  }
}

void line(Point p0, Point p1, Color stroke_color) {
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
    plot_pixel(p, stroke_color);

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

static void ellipse_plot_points(Point center, Point p, int angle_deg, Color c) {
  int xc = center.x, yc = center.y;
  int x = p.x, y = p.y;

  plot_pixel(rotate(center, (Point){xc + x, yc + y}, angle_deg), c);
  plot_pixel(rotate(center, (Point){xc - x, yc + y}, angle_deg), c);
  plot_pixel(rotate(center, (Point){xc + x, yc - y}, angle_deg), c);
  plot_pixel(rotate(center, (Point){xc - x, yc - y}, angle_deg), c);
}

void ellipse(Point center, unsigned int rx, unsigned int ry, int rot_angle_deg,
             Color c) {
  int xc = center.x;
  int yc = center.y;

  int x = 0;
  int y = (int)ry;

  uint32_t rx2 = (uint32_t)rx * (uint32_t)rx;
  uint32_t ry2 = (uint32_t)ry * (uint32_t)ry;

  uint32_t dx = 0;
  uint32_t dy = 2u * rx2 * (uint32_t)y;

  int32_t p1 = (int32_t)(ry2 - rx2 * (uint32_t)ry + (rx2 >> 2));

  while (dx < dy) {
    ellipse_plot_points((Point){xc, yc}, (Point){x, y}, rot_angle_deg, c);

    x++;
    dx += 2u * ry2;

    if (p1 < 0) {
      p1 += (int32_t)(dx + ry2);
    } else {
      y--;
      dy -= 2u * rx2;
      p1 += (int32_t)(dx - dy + ry2);
    }
  }

  uint32_t xp1 = (uint32_t)(x + 1);
  uint32_t ym1 = (uint32_t)(y - 1);

  int32_t p2 = (int32_t)(ry2 * xp1 * xp1 + rx2 * ym1 * ym1 - rx2 * ry2);

  while (y >= 0) {
    ellipse_plot_points((Point){xc, yc}, (Point){x, y}, rot_angle_deg, c);

    y--;
    dy -= 2u * rx2;

    if (p2 > 0) {
      p2 += (int32_t)(rx2 - dy);
    } else {
      x++;
      dx += 2u * ry2;
      p2 += (int32_t)(dx - dy + rx2);
    }
  }
}

void clear(Color color) {
  for (int y = 0; y < g_graphics_state.height; y++) {
    for (int x = 0; x < g_graphics_state.width; x++) {
      uint8_t *fb = (uint8_t *)g_graphics_state.framebuffer + g_y_offsets[y] +
                    x * bytes_per_pixel;
      fb[0] = color.b;
      fb[1] = color.g;
      fb[2] = color.r;
    }
  }
}
