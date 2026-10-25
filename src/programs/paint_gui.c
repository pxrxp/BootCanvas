#include "programs/paint_gui.h"
#include "arch/program.h"
#include "conio.h"
#include "drivers/graphics.h"
#include "drivers/mouse.h"
#include "gui/gui.h"
#include "programs/console.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define TOOL_PENCIL 0
#define TOOL_LINE 1
#define TOOL_RECT 2
#define TOOL_ELLIPSE 3

#define NUM_TOOLS 4
#define NUM_COLORS 8

static bool g_paint_initialized = false;

static int g_current_tool = TOOL_PENCIL;
static Color g_current_color = {255, 255, 255}; // White default

static Point g_canvas_tl = {170, 10};
static Point g_canvas_br = {790, 590};

static bool g_drawing_shape = false;
static Point g_shape_start = {0, 0};
static Point g_last_mouse_pos = {0, 0};
static bool g_last_mouse_left = false;

static Color g_palette[NUM_COLORS] = {
    {255, 255, 255}, // White
    {0, 0, 0},       // Black
    {255, 0, 0},     // Red
    {0, 255, 0},     // Green
    {0, 0, 255},     // Blue
    {255, 255, 0},   // Yellow
    {0, 255, 255},   // Cyan
    {255, 0, 255}    // Magenta
};

static Button g_tool_buttons[NUM_TOOLS] = {{{10, 40},
                                            {150, 70},
                                            "Pencil",
                                            {60, 60, 70},
                                            {80, 80, 95},
                                            {40, 40, 50},
                                            {255, 255, 255},
                                            {120, 120, 130},
                                            false,
                                            false,
                                            TOOL_PENCIL},
                                           {{10, 75},
                                            {150, 105},
                                            "Line",
                                            {60, 60, 70},
                                            {80, 80, 95},
                                            {40, 40, 50},
                                            {255, 255, 255},
                                            {120, 120, 130},
                                            false,
                                            false,
                                            TOOL_LINE},
                                           {{10, 110},
                                            {150, 140},
                                            "Rectangle",
                                            {60, 60, 70},
                                            {80, 80, 95},
                                            {40, 40, 50},
                                            {255, 255, 255},
                                            {120, 120, 130},
                                            false,
                                            false,
                                            TOOL_RECT},
                                           {{10, 145},
                                            {150, 175},
                                            "Ellipse",
                                            {60, 60, 70},
                                            {80, 80, 95},
                                            {40, 40, 50},
                                            {255, 255, 255},
                                            {120, 120, 130},
                                            false,
                                            false,
                                            TOOL_ELLIPSE}};

static Button g_action_buttons[2] = {{{10, 480},
                                      {150, 510},
                                      "Clear Canvas",
                                      {120, 50, 50},
                                      {150, 70, 70},
                                      {90, 30, 30},
                                      {255, 255, 255},
                                      {180, 80, 80},
                                      false,
                                      false,
                                      0},
                                     {{10, 520},
                                      {150, 550},
                                      "Exit to OS",
                                      {50, 50, 60},
                                      {70, 70, 85},
                                      {30, 30, 40},
                                      {255, 255, 255},
                                      {100, 100, 110},
                                      false,
                                      false,
                                      1}};

static void redraw_ui_sidebar(Point mouse_p, bool mouse_pressed) {
  // Sidebar panel
  ui_draw_panel((Point){0, 0}, (Point){160, 600}, (Color){40, 40, 45},
                (Color){80, 80, 90});
  put_text("Paint Tools", (Point){10, 12}, (Color){255, 255, 255},
           (Color){40, 40, 45}, 0, 0);

  // Tool Buttons
  for (int i = 0; i < NUM_TOOLS; i++) {
    if (i == g_current_tool) {
      Color active_bg = {40, 100, 160};
      filled_rectangle(g_tool_buttons[i].top_left,
                       g_tool_buttons[i].bottom_right, active_bg);
      rectangle(g_tool_buttons[i].top_left, g_tool_buttons[i].bottom_right, 0,
                (Color){255, 255, 255});
      put_text(g_tool_buttons[i].label,
               (Point){g_tool_buttons[i].top_left.x + 10,
                       g_tool_buttons[i].top_left.y + 8},
               (Color){255, 255, 255}, active_bg, 0, 0);
    } else {
      ui_draw_button(&g_tool_buttons[i], mouse_p, mouse_pressed);
    }
  }

  // Color Swatches Header
  put_text("Palette", (Point){10, 195}, (Color){255, 255, 255},
           (Color){40, 40, 45}, 0, 0);

  // Render Color Grid
  for (int i = 0; i < NUM_COLORS; i++) {
    int x = 10 + (i % 4) * 35;
    int y = 220 + (i / 4) * 35;
    Point c_tl = {x, y};
    Point c_br = {x + 30, y + 30};

    filled_rectangle(c_tl, c_br, g_palette[i]);
    Color border_col = (g_current_color.r == g_palette[i].r &&
                        g_current_color.g == g_palette[i].g &&
                        g_current_color.b == g_palette[i].b)
                           ? (Color){255, 255, 255}
                           : (Color){0, 0, 0};
    rectangle(c_tl, c_br, 0, border_col);
  }

  // Action Buttons
  ui_draw_button(&g_action_buttons[0], mouse_p, mouse_pressed);
  ui_draw_button(&g_action_buttons[1], mouse_p, mouse_pressed);
}

void PAINT_init(void) {
  clrscr();
  filled_rectangle((Point){0, 0}, (Point){800, 600}, (Color){20, 20, 25});
  filled_rectangle(g_canvas_tl, g_canvas_br, (Color){0, 0, 0});
  rectangle(g_canvas_tl, g_canvas_br, 0, (Color){150, 150, 160});

  redraw_ui_sidebar((Point){0, 0}, false);
  g_paint_initialized = true;
}

void PAINT_run(bool keyboard_changed, bool mouse_changed) {
  if (!g_paint_initialized) {
    PAINT_init();
  }

  if (keyboard_changed && kbhit()) {
    int8_t c = getch();
    if (c == 27) { // ESC
      g_paint_initialized = false;
      clrscr();
      set_current_program((Program){NULL, CONSOLE_run});
      return;
    }
  }

  const MouseState *mouse = get_mouse_state();
  Point mouse_p = {mouse->x, mouse->y};
  bool mouse_pressed = mouse->left_button;
  bool mouse_released = g_last_mouse_left && !mouse_pressed;

  redraw_ui_sidebar(mouse_p, mouse_pressed);

  // Check Sidebar Interactions
  if (mouse_released) {
    for (int i = 0; i < NUM_TOOLS; i++) {
      if (ui_button_clicked(&g_tool_buttons[i], mouse_p, true)) {
        g_current_tool = i;
      }
    }

    // Color Selection
    for (int i = 0; i < NUM_COLORS; i++) {
      int x = 10 + (i % 4) * 35;
      int y = 220 + (i / 4) * 35;
      if (ui_point_in_rect(mouse_p, (Point){x, y}, (Point){x + 30, y + 30})) {
        g_current_color = g_palette[i];
      }
    }

    // Clear Canvas Button
    if (ui_button_clicked(&g_action_buttons[0], mouse_p, true)) {
      filled_rectangle(g_canvas_tl, g_canvas_br, (Color){0, 0, 0});
      rectangle(g_canvas_tl, g_canvas_br, 0, (Color){150, 150, 160});
    }

    // Exit Button
    if (ui_button_clicked(&g_action_buttons[1], mouse_p, true)) {
      g_paint_initialized = false;
      clrscr();
      set_current_program((Program){NULL, CONSOLE_run});
      return;
    }
  }

  // Canvas Drawing Logic
  bool inside_canvas =
      ui_point_in_rect(mouse_p, (Point){g_canvas_tl.x + 1, g_canvas_tl.y + 1},
                       (Point){g_canvas_br.x - 1, g_canvas_br.y - 1});

  if (inside_canvas) {
    if (g_current_tool == TOOL_PENCIL) {
      if (mouse_pressed) {
        if (!g_last_mouse_left) {
          g_last_mouse_pos = mouse_p;
        }

        // Draw a smooth 3x3 stroke by connecting the previous position to the
        // current position
        for (int dx = -1; dx <= 1; dx++) {
          for (int dy = -1; dy <= 1; dy++) {
            line((Point){g_last_mouse_pos.x + dx, g_last_mouse_pos.y + dy},
                 (Point){mouse_p.x + dx, mouse_p.y + dy}, g_current_color);
          }
        }

        g_last_mouse_pos = mouse_p;
      }
    } else { // Shape Tools (Line, Rect, Ellipse)
      if (mouse_pressed && !g_last_mouse_left) {
        g_drawing_shape = true;
        g_shape_start = mouse_p;
      } else if (mouse_released && g_drawing_shape) {
        g_drawing_shape = false;

        if (g_current_tool == TOOL_LINE) {
          line(g_shape_start, mouse_p, g_current_color);
        } else if (g_current_tool == TOOL_RECT) {
          // Normalize rectangle points for any drag direction
          Point p1 = {
              (g_shape_start.x < mouse_p.x) ? g_shape_start.x : mouse_p.x,
              (g_shape_start.y < mouse_p.y) ? g_shape_start.y : mouse_p.y};
          Point p2 = {
              (g_shape_start.x > mouse_p.x) ? g_shape_start.x : mouse_p.x,
              (g_shape_start.y > mouse_p.y) ? g_shape_start.y : mouse_p.y};
          rectangle(p1, p2, 0, g_current_color);
        } else if (g_current_tool == TOOL_ELLIPSE) {
          // Find bounding box extremes regardless of drag direction
          int min_x =
              (g_shape_start.x < mouse_p.x) ? g_shape_start.x : mouse_p.x;
          int max_x =
              (g_shape_start.x > mouse_p.x) ? g_shape_start.x : mouse_p.x;
          int min_y =
              (g_shape_start.y < mouse_p.y) ? g_shape_start.y : mouse_p.y;
          int max_y =
              (g_shape_start.y > mouse_p.y) ? g_shape_start.y : mouse_p.y;

          int rx = (max_x - min_x) / 2;
          int ry = (max_y - min_y) / 2;

          Point center = {min_x + rx, min_y + ry};
          ellipse(center, rx, ry, 0, g_current_color);
        }
      }
    }
  }

  g_last_mouse_left = mouse_pressed;
}
