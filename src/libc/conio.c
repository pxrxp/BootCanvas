#include "drivers/font.h"
#include "drivers/graphics.h"
#include "drivers/keyboard.h"
#include "drivers/scancode.h"
#include <conio.h>
#include <stdbool.h>
#include <string.h>

static ConsoleState g_console_state;

void init_console(Point top_left) {
  g_console_state.current_col = 0;
  g_console_state.current_row = 0;
  g_console_state.current_background_color = (Color){0, 0, 0};
  g_console_state.current_foreground_color = (Color){255, 255, 255};
  g_console_state.top_left = (Point){0, 0};

  recalculate_dimensions();

  memset(g_console_state.cells, 0, sizeof(g_console_state.cells));
}

bool kbhit() { return get_keyboard_state()->key_pressed; }

int8_t getch() {
  ScanCode last_scancode = get_keyboard_state()->last_scancode;
  if (is_ascii_key(last_scancode)) {
    return scancode_to_ascii[last_scancode];
  }
  return -1;
}

int8_t getche(void) {
  char c = getch();
  if (c != -1)
    putch(c);
  return c;
}

void fix_console_state(void) {
    if (g_console_state.current_col >= g_console_state.max_cols) {
        g_console_state.current_col = 0;
        g_console_state.current_row++;
    }

    if (g_console_state.current_row >= g_console_state.max_rows) {
        g_console_state.current_row = g_console_state.max_rows - 1;
    }
}

void putch(char c) {
    if (c == '\n') {
        g_console_state.current_col = 0;
        g_console_state.current_row++;
        fix_console_state();
        return;
    }

    if (c == '\r') {
        g_console_state.current_col = 0;
        return;
    }

    if (c == '\b') {
        if (g_console_state.current_col > 0) {
            g_console_state.current_col--;
            g_console_state.cells[g_console_state.current_row][g_console_state.current_col] = 
                (Cell){' ', g_console_state.current_foreground_color, g_console_state.current_background_color};
        }
        return;
    }

    fix_console_state();

    g_console_state.cells[g_console_state.current_row][g_console_state.current_col] =
        (Cell){c, g_console_state.current_foreground_color, g_console_state.current_background_color};

    g_console_state.current_col++;

    fix_console_state();
}

char *cgets(char *s) {
  char *str = s;
  char input;
  while ((input = getch()) != '\n') {
    *str++ = input;
  }
  *str = '\0';
  return s;
}

void cputs(const char *s) {
  while (*s) {
    putch(*s);
  }
}

void set_background_color(Color c) {
  g_console_state.current_background_color = c;
}

void set_foreground_color(Color c) {
  g_console_state.current_foreground_color = c;
}

void set_max_rows(uint16_t r) { g_console_state.max_rows = r; }

void set_max_cols(uint16_t c) { g_console_state.max_cols = c; }

void set_top_left(Point tl) { g_console_state.top_left = tl; }

void recalculate_dimensions() {
  const GraphicsState *graphics_state = get_graphics_state();
  g_console_state.max_cols =
      (graphics_state->width - g_console_state.top_left.x) / CHARACTER_WIDTH;
  g_console_state.max_rows =
      (graphics_state->height - g_console_state.top_left.y) / CHARACTER_HEIGHT;
}

const ConsoleState *get_console_state() { return &g_console_state; }

void render_console(void) {
  for (int r = 0; r < g_console_state.max_rows; r++) {
    Point p = {g_console_state.top_left.x,
               g_console_state.top_left.y + (r * CHARACTER_HEIGHT)};

    for (int c = 0; c < g_console_state.max_cols; c++) {
      Cell cell = g_console_state.cells[r][c];
      if (cell.ascii_character != 0) {
        put_char(cell.ascii_character, &p, g_console_state.top_left.x,
                 cell.foreground, cell.background, 0, 0);
      } else {
        p.x += CHARACTER_WIDTH;
      }
    }
  }
}
