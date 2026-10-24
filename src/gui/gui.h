#ifndef GUI_H
#define GUI_H

#include "drivers/graphics.h"
#include <stdbool.h>

typedef struct {
    Point top_left;
    Point bottom_right;
    const char *label;
    Color bg_normal;
    Color bg_hover;
    Color bg_pressed;
    Color text_color;
    Color border_color;
    bool is_hovered;
    bool is_pressed;
    int id;
} Button;

bool ui_point_in_rect(Point p, Point top_left, Point bottom_right);
void ui_draw_panel(Point top_left, Point bottom_right, Color bg, Color border);
void ui_draw_window_frame(const char *title, Point top_left, Point bottom_right);
void ui_draw_button(Button *btn, Point mouse_p, bool mouse_pressed);
bool ui_button_clicked(Button *btn, Point mouse_p, bool mouse_released);

#endif
