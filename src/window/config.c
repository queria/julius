#include "config.h"

#include "core/config.h"
#include "core/image_group.h"
#include "core/string.h"
#include "graphics/button.h"
#include "graphics/generic_button.h"
#include "graphics/labeled_button.h"
#include "graphics/graphics.h"
#include "graphics/image.h"
#include "graphics/panel.h"
#include "graphics/text.h"
#include "graphics/window.h"
#include "window/main_menu.h"

#define NUM_CHECKBOXES 18
#define NUM_BOTTOM_BUTTONS 3

static void toggle_switch(int id, int param2);
static void button_reset_defaults(int param1, int param2);
static void button_close(int save, int param2);

static labeled_button checkbox_buttons[] = {
    { 20, 0, 20, 20, toggle_switch, button_none, CONFIG_UI_SHOW_INTRO_VIDEO, 0, "Play intro videos" },
    { 20, 0, 20, 20, toggle_switch, button_none, CONFIG_UI_SIDEBAR_INFO, 0, "Extra information in the control panel" },
    { 20, 0, 20, 20, toggle_switch, button_none, CONFIG_UI_SMOOTH_SCROLLING, 0, "Enable smooth scrolling" },
    { 20, 0, 20, 20, toggle_switch, button_none, CONFIG_UI_WALKER_WAYPOINTS, 0, "Draw walker waypoints on overlay after right clicking on a building." },
    { 20, 0, 20, 20, toggle_switch, button_none, CONFIG_UI_VISUAL_FEEDBACK_ON_DELETE, 0, "Improve visual feedback when clearing land" },
    { 20, 0, 20, 20, toggle_switch, button_none, CONFIG_GP_FIX_IMMIGRATION_BUG, 0, "Fix immigration on very hard" },
    { 20, 0, 20, 20, toggle_switch, button_none, CONFIG_GP_FIX_100_YEAR_GHOSTS, 0, "Fix 100-year-old ghosts" },
    { 20, 0, 20, 20, toggle_switch, button_none, CONFIG_GP_CH_GRANDFESTIVAL, 0, "Grand festivals allow extra blessing from a god" },
    { 20, 0, 20, 20, toggle_switch, button_none, CONFIG_GP_CH_JEALOUS_GODS, 0, "Disable jealousness of gods" },
    { 20, 0, 20, 20, toggle_switch, button_none, CONFIG_GP_CH_GLOBAL_LABOUR, 0, "Enable global labour pool" },
    { 20, 0, 20, 20, toggle_switch, button_none, CONFIG_GP_CH_SCHOOL_WALKERS, 0, "Extend school walkers range" },
    { 20, 0, 20, 20, toggle_switch, button_none, CONFIG_GP_CH_RETIRE_AT_60, 0, "Change citizens' retirement age from 50 to 60" },
    { 20, 0, 20, 20, toggle_switch, button_none, CONFIG_GP_CH_FIXED_WORKERS, 0, "Fixed worker pool - 38% of population" },
    { 20, 0, 20, 20, toggle_switch, button_none, CONFIG_GP_CH_EXTRA_FORTS, 0, "Allow building 3 extra forts" },
    { 20, 0, 20, 20, toggle_switch, button_none, CONFIG_GP_CH_WOLVES_BLOCK, 0, "Block building around wolves" },
    { 20, 0, 20, 20, toggle_switch, button_none, CONFIG_GP_CH_DYNAMIC_GRANARIES, 0, "Dynamic granaries" },
    { 20, 0, 20, 20, toggle_switch, button_none, CONFIG_GP_CH_MORE_STOCKPILE, 0, "Houses stockpile more goods from market" },
    { 20, 0, 20, 20, toggle_switch, button_none, CONFIG_GP_CH_NO_BUYER_DISTRIBUTION, 0, "Buying market ladies don't distribute goods" },
};

static generic_button bottom_buttons[] = {
    { 200, 430, 150, 30, button_reset_defaults, button_none },
    { 410, 430, 100, 30, button_close, button_none, 0 },
    { 520, 430, 100, 30, button_close, button_none, 1 },
};

static const char *bottom_button_texts[] = {
    "Reset defaults",
    "Cancel",
    "OK"
};

static struct {
    int focus_button;
    int bottom_focus_button;
    int values[CONFIG_MAX_ENTRIES];
} data;

static void init(void)
{
    int next_button_y = 72;
    int button_row_height = 24;

    for (int i = 0; i < NUM_CHECKBOXES; i++) {
        config_key key = checkbox_buttons[i].parameter1;
        data.values[key] = config_get(key);

        if (checkbox_buttons[i].parameter1 == CONFIG_GP_FIX_IMMIGRATION_BUG) {
            // there is Section header above this button so move it by one extra row
            next_button_y += button_row_height;
        }
        checkbox_buttons[i].y = next_button_y;
        next_button_y += button_row_height;
    }
}

static void draw_background(void)
{
    graphics_clear_screen();

    image_draw_fullscreen_background(image_group(GROUP_CONFIG));

    graphics_in_dialog();
    outer_panel_draw(0, 0, 40, 30);

    text_draw_centered(string_from_ascii("Julius configuration options"), 16, 16, 608, FONT_LARGE_BLACK, 0);

    text_draw(string_from_ascii("User interface changes"), 20, 53, FONT_NORMAL_BLACK, 0);
    text_draw(string_from_ascii("Gameplay changes"), 20, 197, FONT_NORMAL_BLACK, 0);

    for (int i = 0; i < NUM_CHECKBOXES; i++) {
        labeled_button *btn = &checkbox_buttons[i];
        if (data.values[btn->parameter1]) {
            text_draw(string_from_ascii("x"), btn->x + 6, btn->y + 3, FONT_NORMAL_BLACK, 0);
        }
        text_draw(string_from_ascii(btn->label), 50, btn->y + 5, FONT_NORMAL_BLACK, 0);
    }

    for (int i = 0; i < NUM_BOTTOM_BUTTONS; i++) {
        text_draw_centered(string_from_ascii(bottom_button_texts[i]), bottom_buttons[i].x, bottom_buttons[i].y + 9, bottom_buttons[i].width, FONT_NORMAL_BLACK, 0);
    }

    graphics_reset_dialog();
}

static void draw_foreground(void)
{
    graphics_in_dialog();
    for (int i = 0; i < NUM_CHECKBOXES; i++) {
        labeled_button *btn = &checkbox_buttons[i];
        button_border_draw(btn->x, btn->y, btn->width, btn->height, data.focus_button == i + 1);
    }
    for (int i = 0; i < NUM_BOTTOM_BUTTONS; i++) {
        button_border_draw(bottom_buttons[i].x, bottom_buttons[i].y, bottom_buttons[i].width, bottom_buttons[i].height, data.bottom_focus_button == i + 1);
    }
    graphics_reset_dialog();
}

static void handle_mouse(const mouse *m)
{
    const mouse *m_dialog = mouse_in_dialog(m);
    labeled_buttons_handle_mouse(m_dialog, 0, 0, checkbox_buttons, NUM_CHECKBOXES, &data.focus_button);
    generic_buttons_handle_mouse(m_dialog, 0, 0, bottom_buttons, NUM_BOTTOM_BUTTONS, &data.bottom_focus_button);
}

static void toggle_switch(int key, int param2)
{
    data.values[key] = 1 - data.values[key];
    window_invalidate();
}

static void button_reset_defaults(int param1, int param2)
{
    config_set_defaults();
    init();
    window_invalidate();
}

static void button_close(int save, int param2)
{
    if (save) {
        for (int i = 0; i < NUM_CHECKBOXES; i++) {
            config_key key = checkbox_buttons[i].parameter1;
            config_set(key, data.values[key]);
        }
    }
    window_main_menu_show(0);
}

void window_config_show()
{
    window_type window = {
        WINDOW_CONFIG,
        draw_background,
        draw_foreground,
        handle_mouse
    };
    init();
    window_show(&window);
}
