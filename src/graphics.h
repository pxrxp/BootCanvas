#include <stdint.h>

static uint32_t *g_framebuffer;
static uint32_t g_width;
static uint32_t g_height;
static uint32_t g_bpp;
static uint32_t g_pitch;

typedef struct {
  uint8_t r;
  uint8_t g;
  uint8_t b;
} Color;

typedef struct Point {
  int x;
  int y;
} Point;

void init_graphics(uint32_t framebuffer, uint32_t width, uint32_t height,
                   uint32_t bpp, uint32_t pitch);

void plot_pixel(Point point, Color color);
void line(Point p0, Point p1, Color c);
void rectangle(Point top_left, Point bottom_right, Color stroke_color);
