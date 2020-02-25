#ifndef GRAPHICS_LABELED_BUTTON_H
#define GRAPHICS_LABELED_BUTTON_H

#include "graphics/button.h"
#include "input/mouse.h"

typedef struct {
    short x;
    short y;
    short width;
    short height;
    void (*left_click_handler)(int param1, int param2);
    void (*right_click_handler)(int param1, int param2);
    int parameter1;
    int parameter2;
    const char *label;
} labeled_button;

int labeled_buttons_handle_mouse(const mouse *m, int x, int y, labeled_button *buttons, int num_buttons, int *focus_button_id);

#endif // GRAPHICS_LABELED_BUTTON_H
