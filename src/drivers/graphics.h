#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <stdint.h>

typedef struct {
  uint8_t r;
  uint8_t g;
  uint8_t b;
} Color;

typedef struct Point {
  int x;
  int y;
} Point;

typedef struct {
  uint32_t *framebuffer;
  uint32_t width;
  uint32_t height;
  uint32_t bits_per_pixel;
  uint32_t pitch;
} GraphicsState;

void init_graphics(uint32_t framebuffer, uint32_t width, uint32_t height,
                   uint32_t bits_per_pixel, uint32_t pitch);
const GraphicsState *get_graphics_state(void);

Color get_pixel(Point point);
void plot_pixel(Point point, Color color);
void line(Point p0, Point p1, Color c);
void rectangle(Point top_left, Point bottom_right, int rot_angle_deg,
               Color stroke_color);
void filled_rectangle(Point top_left, Point bottom_right, Color color);
void ellipse(Point center, unsigned int rx, unsigned int ry, int rot_angle_deg,
             Color stroke_color);
void put_char(char c, Point *p, int start_x, Color foreground, Color background,
              unsigned int x_space, unsigned int y_space);
void put_text(const char *text, Point top_left, Color foreground,
              Color background, unsigned int x_space, unsigned int y_space);
void clear(Color color);
void swap_buffers(void);

#endif
