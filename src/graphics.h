#include <stdint.h>

static uint32_t* g_framebuffer;
static uint32_t g_width;
static uint32_t g_height;
static uint32_t g_bpp;
static uint32_t g_pitch;

void init_graphics(uint32_t framebuffer, uint32_t width, uint32_t height, uint32_t bpp, uint32_t pitch);

void put_pixel(int x, int y, uint8_t r, uint8_t g, uint8_t b);
