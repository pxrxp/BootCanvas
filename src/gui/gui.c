#include "gui/gui.h"
#include "drivers/font.h"
#include "drivers/graphics.h"
#include <string.h>

bool ui_point_in_rect(Point p, Point top_left, Point bottom_right) {
    return (p.x >= top_left.x && p.x < bottom_right.x &&
            p.y >= top_left.y && p.y < bottom_right.y);
}

void ui_draw_panel(Point top_left, Point bottom_right, Color bg, Color border) {
    filled_rectangle(top_left, bottom_right, bg);
    rectangle(top_left, bottom_right, 0, border);
}

void ui_draw_window_frame(const char *title, Point top_left, Point bottom_right) {
    Color bg = {35, 35, 40};
    Color title_bg = {50, 80, 140};
    Color border = {90, 90, 100};
    Color white = {255, 255, 255};

    // Window canvas background
    filled_rectangle(top_left, bottom_right, bg);

    // Title bar (height 24px)
    Point title_br = {bottom_right.x, top_left.y + 24};
    filled_rectangle(top_left, title_br, title_bg);

    // Title label
    Point text_p = {top_left.x + 8, top_left.y + 8};
    put_text(title, text_p, white, title_bg, 0, 0);

    // Outer border
    rectangle(top_left, bottom_right, 0, border);
}

void ui_draw_button(Button *btn, Point mouse_p, bool mouse_pressed) {
    btn->is_hovered = ui_point_in_rect(mouse_p, btn->top_left, btn->bottom_right);
    btn->is_pressed = btn->is_hovered && mouse_pressed;

    Color bg = btn->bg_normal;
    if (btn->is_pressed) {
        bg = btn->bg_pressed;
    } else if (btn->is_hovered) {
        bg = btn->bg_hover;
    }

    filled_rectangle(btn->top_left, btn->bottom_right, bg);
    rectangle(btn->top_left, btn->bottom_right, 0, btn->border_color);

    if (btn->label && btn->label[0] != '\0') {
        size_t len = strlen(btn->label);
        uint32_t width = btn->bottom_right.x - btn->top_left.x;
        uint32_t height = btn->bottom_right.y - btn->top_left.y;

        int text_width = (int)len * CHARACTER_WIDTH;
        int text_height = CHARACTER_HEIGHT;

        int text_x = btn->top_left.x + (int)(width - text_width) / 2;
        int text_y = btn->top_left.y + (int)(height - text_height) / 2;

        if (text_x < btn->top_left.x) text_x = btn->top_left.x + 2;
        if (text_y < btn->top_left.y) text_y = btn->top_left.y + 2;

        if (btn->is_pressed) {
            text_x += 1;
            text_y += 1;
        }

        Point tp = {text_x, text_y};
        put_text(btn->label, tp, btn->text_color, bg, 0, 0);
    }
}

bool ui_button_clicked(Button *btn, Point mouse_p, bool mouse_released) {
    return ui_point_in_rect(mouse_p, btn->top_left, btn->bottom_right) && mouse_released;
}
