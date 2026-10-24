#include "programs/calc_gui.h"
#include "arch/program.h"
#include "conio.h"
#include "drivers/font.h"
#include "drivers/graphics.h"
#include "drivers/mouse.h"
#include "gui/gui.h"
#include "programs/console.h"
#include "stdlib.h"
#include "string.h"

#define GRID_ROWS 4
#define GRID_COLS 4
#define TOTAL_BUTTONS 16

static bool g_calc_initialized = false;
static char g_display_buf[32] = "0";
static int g_first_operand = 0;
static char g_pending_op = '\0';
static bool g_clear_on_next_digit = false;
static bool g_last_mouse_left = false;

static Point g_win_tl = {220, 80};
static Point g_win_br = {580, 500};

static Button g_calc_buttons[TOTAL_BUTTONS];
static const char *g_btn_labels[TOTAL_BUTTONS] = {"7", "8", "9", "/", "4", "5",
                                                  "6", "*", "1", "2", "3", "-",
                                                  "C", "0", "=", "+"};

static void init_button_grid(void) {
  int start_x = g_win_tl.x + 20;
  int start_y = g_win_tl.y + 100;
  int btn_w = 75;
  int btn_h = 75;
  int gap = 10;

  for (int i = 0; i < TOTAL_BUTTONS; i++) {
    int r = i / GRID_COLS;
    int c = i % GRID_COLS;

    int x1 = start_x + c * (btn_w + gap);
    int y1 = start_y + r * (btn_h + gap);

    Color bg = {60, 60, 70};
    Color bg_h = {80, 80, 95};
    Color bg_p = {40, 40, 50};

    // Operator colors
    if (c == 3 || strcmp(g_btn_labels[i], "C") == 0 ||
        strcmp(g_btn_labels[i], "=") == 0) {
      bg = (Color){180, 90, 30};
      bg_h = (Color){210, 110, 40};
      bg_p = (Color){140, 70, 20};
    }

    g_calc_buttons[i] = (Button){
        {x1, y1}, {x1 + btn_w, y1 + btn_h}, g_btn_labels[i], bg,    bg_h,
        bg_p,     {255, 255, 255},          {100, 100, 110}, false, false,
        i};
  }
}

static void update_calc_display(void) {
  Point disp_tl = {g_win_tl.x + 20, g_win_tl.y + 40};
  Point disp_br = {g_win_br.x - 20, g_win_tl.y + 85};

  filled_rectangle(disp_tl, disp_br, (Color){10, 20, 10});
  rectangle(disp_tl, disp_br, 0, (Color){80, 120, 80});

  size_t len = strlen(g_display_buf);
  int text_x = disp_br.x - (int)len * CHARACTER_WIDTH - 12;
  if (text_x < disp_tl.x + 8)
    text_x = disp_tl.x + 8;

  put_text(g_display_buf, (Point){text_x, disp_tl.y + 12}, (Color){50, 255, 50},
           (Color){10, 20, 10}, 0, 0);
}

static void handle_calc_input(const char *label) {
  if (label[0] >= '0' && label[0] <= '9') {
    if (g_clear_on_next_digit || strcmp(g_display_buf, "0") == 0) {
      g_display_buf[0] = label[0];
      g_display_buf[1] = '\0';
      g_clear_on_next_digit = false;
    } else {
      size_t len = strlen(g_display_buf);
      if (len < 12) {
        g_display_buf[len] = label[0];
        g_display_buf[len + 1] = '\0';
      }
    }
  } else if (strcmp(label, "C") == 0) {
    g_display_buf[0] = '0';
    g_display_buf[1] = '\0';
    g_first_operand = 0;
    g_pending_op = '\0';
    g_clear_on_next_digit = false;
  } else if (label[0] == '+' || label[0] == '-' || label[0] == '*' ||
             label[0] == '/') {
    g_first_operand = atoi(g_display_buf);
    g_pending_op = label[0];
    g_clear_on_next_digit = true;
  } else if (strcmp(label, "=") == 0 && g_pending_op != '\0') {
    int second_operand = atoi(g_display_buf);
    int result = 0;
    bool err = false;

    switch (g_pending_op) {
    case '+':
      result = g_first_operand + second_operand;
      break;
    case '-':
      result = g_first_operand - second_operand;
      break;
    case '*':
      result = g_first_operand * second_operand;
      break;
    case '/':
      if (second_operand == 0)
        err = true;
      else
        result = g_first_operand / second_operand;
      break;
    }

    if (err) {
      strcpy(g_display_buf, "DIV ERROR");
    } else {
      itoa(result, g_display_buf);
    }

    g_pending_op = '\0';
    g_clear_on_next_digit = true;
  }
}

void CALC_GUI_init(void) {
  clrscr();
  ui_draw_window_frame("GUI Calculator", g_win_tl, g_win_br);
  init_button_grid();
  update_calc_display();
  g_calc_initialized = true;
}

void CALC_GUI_run(bool keyboard_changed, bool mouse_changed) {
  if (!g_calc_initialized) {
    CALC_GUI_init();
  }

  if (keyboard_changed && kbhit()) {
    int8_t c = getch();
    if (c == 27) { // ESC exits back to OS Console
      g_calc_initialized = false;
      clrscr();
      set_current_program((Program){NULL, CONSOLE_run});
      return;
    }
  }

  const MouseState *mouse = get_mouse_state();
  Point mouse_p = {mouse->x, mouse->y};
  bool mouse_pressed = mouse->left_button;
  bool mouse_released = g_last_mouse_left && !mouse_pressed;

  // Render Calculator UI
  ui_draw_window_frame("GUI Calculator", g_win_tl, g_win_br);
  update_calc_display();

  for (int i = 0; i < TOTAL_BUTTONS; i++) {
    ui_draw_button(&g_calc_buttons[i], mouse_p, mouse_pressed);

    if (ui_button_clicked(&g_calc_buttons[i], mouse_p, mouse_released)) {
      handle_calc_input(g_calc_buttons[i].label);
    }
  }

  g_last_mouse_left = mouse_pressed;
}
