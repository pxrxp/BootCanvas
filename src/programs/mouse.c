#include "programs/mouse.h"
#include "drivers/graphics.h"
#include "drivers/mouse.h"

static Color g_mouse_cursor_color = {255, 255, 255};

void MOUSE_run() {
  const MouseState *mouse_state = get_mouse_state();
  plot_pixel((Point){mouse_state->x, mouse_state->y}, g_mouse_cursor_color);
}

void MOUSE_set_cursor_color(Color c) { g_mouse_cursor_color = c; }
