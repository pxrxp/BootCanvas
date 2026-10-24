#include "programs/mouse.h"
#include "drivers/graphics.h"
#include "drivers/mouse.h"

static Color g_mouse_cursor_color = {255, 255, 255};
static Color g_mouse_border_color = {0, 0, 0};

static const uint8_t cursor_sprite[12][8] = {
    {2, 0, 0, 0, 0, 0, 0, 0},
    {2, 2, 0, 0, 0, 0, 0, 0},
    {2, 1, 2, 0, 0, 0, 0, 0},
    {2, 1, 1, 2, 0, 0, 0, 0},
    {2, 1, 1, 1, 2, 0, 0, 0},
    {2, 1, 1, 1, 1, 2, 0, 0},
    {2, 1, 1, 1, 1, 1, 2, 0},
    {2, 1, 1, 1, 2, 2, 2, 0},
    {2, 1, 2, 1, 2, 0, 0, 0},
    {2, 2, 0, 2, 1, 2, 0, 0},
    {0, 0, 0, 0, 2, 1, 2, 0},
    {0, 0, 0, 0, 0, 2, 2, 0},
};

static int g_old_mx = -1;
static int g_old_my = -1;
static Color g_saved_bg[12][8];
static bool g_has_saved_bg = false;

void MOUSE_restore_bg(void) {
  if (!g_has_saved_bg) return;

  for (int row = 0; row < 12; row++) {
    for (int col = 0; col < 8; col++) {
      if (cursor_sprite[row][col] != 0) {
        plot_pixel((Point){g_old_mx + col, g_old_my + row}, g_saved_bg[row][col]);
      }
    }
  }
  g_has_saved_bg = false;
}

void MOUSE_run(void) {
  const MouseState *mouse_state = get_mouse_state();
  int mx = mouse_state->x;
  int my = mouse_state->y;

  // Save current background under new cursor position
  for (int row = 0; row < 12; row++) {
    for (int col = 0; col < 8; col++) {
      if (cursor_sprite[row][col] != 0) {
        g_saved_bg[row][col] = get_pixel((Point){mx + col, my + row});
      }
    }
  }
  g_old_mx = mx;
  g_old_my = my;
  g_has_saved_bg = true;

  // Draw cursor sprite
  for (int row = 0; row < 12; row++) {
    for (int col = 0; col < 8; col++) {
      uint8_t pixel_type = cursor_sprite[row][col];
      if (pixel_type == 1) {
        plot_pixel((Point){mx + col, my + row}, g_mouse_cursor_color);
      } else if (pixel_type == 2) {
        plot_pixel((Point){mx + col, my + row}, g_mouse_border_color);
      }
    }
  }
}

void MOUSE_set_cursor_color(Color c) { g_mouse_cursor_color = c; }
