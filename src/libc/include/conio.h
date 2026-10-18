// THIS FILE DOES NOT REPLICATE conio.h OF MS-DOS
// FUNCTION NAMES ARE ONLY SIMILAR

#ifndef CONIO_H
#define CONIO_H
#include "drivers/graphics.h"
#include <stdbool.h>
#include <stdint.h>

#define CONSOLE_COLS 240
#define CONSOLE_ROWS 128

typedef struct {
  char ascii_character;
  Color foreground;
  Color background;
} Cell;

typedef struct {
  uint16_t current_row;
  uint16_t current_col;
  uint16_t max_rows;
  uint16_t max_cols;

  Color current_background_color;
  Color current_foreground_color;

  Point top_left;

  Cell cells[CONSOLE_COLS][CONSOLE_ROWS + 1];
} ConsoleState;

void init_console(Point top_left);

bool kbhit(void);
int8_t getch(void);
int8_t getche(void);
void putch(char c);
char *cgets(char *s);
void cputs(const char *s);

void set_background_color(Color c);
void set_foreground_color(Color c);
void set_max_rows(uint16_t r);
void set_max_cols(uint16_t c);
void set_top_left(Point tl);
void recalculate_dimensions(void);

const ConsoleState *get_console_state(void);
void render_console(void);

#endif
