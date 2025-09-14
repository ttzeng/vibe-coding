#ifndef MENU_SYSTEM_H
#define MENU_SYSTEM_H

#include "ssd1306.h"
#include "app_config.h"

typedef enum {
    MENU_MAIN = 0,
    MENU_SETTINGS,
    MENU_DISPLAY,
    MENU_NETWORK,
    MENU_SYSTEM,
    MENU_ABOUT,
    MENU_MAX
} menu_type_t;

typedef struct {
    char title[32];
    void (*action)(void);
    menu_type_t submenu;
    bool has_submenu;
} menu_item_t;

// Menu System API
esp_err_t menu_system_init(void);
void menu_system_display(ssd1306_handle_t display);
void menu_system_navigate_up(void);
void menu_system_navigate_down(void);
void menu_system_select(void);
void menu_system_back(void);
bool menu_system_is_active(void);
void menu_system_activate(bool active);

#endif // MENU_SYSTEM_H
