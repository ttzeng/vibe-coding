#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include "menu_system.h"
#include "esp_log.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "nvs_flash.h"
#include "nvs.h"

static const char *TAG = "MENU";

static menu_type_t current_menu = MENU_MAIN;
static int current_selection = 0;
static int scroll_offset = 0;
static bool menu_active = false;
static const int ITEMS_PER_PAGE = 4;

// Menu action functions
static void action_brightness_up(void);
static void action_brightness_down(void);
static void action_wifi_scan(void);
static void action_wifi_disconnect(void);
static void action_system_info(void);
static void action_factory_reset(void);
static void action_reboot(void);

// Menu definitions
static const menu_item_t main_menu[] = {
    {"Display Settings", NULL, MENU_DISPLAY, true},
    {"Network Settings", NULL, MENU_NETWORK, true},
    {"System Settings", NULL, MENU_SYSTEM, true},
    {"About", NULL, MENU_ABOUT, true},
};

static const menu_item_t display_menu[] = {
    {"Brightness +", action_brightness_up, 0, false},
    {"Brightness -", action_brightness_down, 0, false},
    {"< Back", NULL, MENU_MAIN, true},
};

static const menu_item_t network_menu[] = {
    {"WiFi Scan", action_wifi_scan, 0, false},
    {"Disconnect WiFi", action_wifi_disconnect, 0, false},
    {"< Back", NULL, MENU_MAIN, true},
};

static const menu_item_t system_menu[] = {
    {"System Info", action_system_info, 0, false},
    {"Factory Reset", action_factory_reset, 0, false},
    {"Reboot", action_reboot, 0, false},
    {"< Back", NULL, MENU_MAIN, true},
};

static const menu_item_t about_menu[] = {
    {"ESP32-C3 OLED", NULL, 0, false},
    {"Version: " APP_VERSION, NULL, 0, false},
    {"By: Your Name", NULL, 0, false},
    {"< Back", NULL, MENU_MAIN, true},
};

// Menu structure
typedef struct {
    const menu_item_t *items;
    int item_count;
} menu_definition_t;

static const menu_definition_t menus[MENU_MAX] = {
    {main_menu, sizeof(main_menu) / sizeof(menu_item_t)},
    {NULL, 0}, // MENU_SETTINGS - unused
    {display_menu, sizeof(display_menu) / sizeof(menu_item_t)},
    {network_menu, sizeof(network_menu) / sizeof(menu_item_t)},
    {system_menu, sizeof(system_menu) / sizeof(menu_item_t)},
    {about_menu, sizeof(about_menu) / sizeof(menu_item_t)},
};

esp_err_t menu_system_init(void)
{
    current_menu = MENU_MAIN;
    current_selection = 0;
    scroll_offset = 0;
    menu_active = false;
    
    ESP_LOGI(TAG, "Menu system initialized");
    return ESP_OK;
}

void menu_system_display(ssd1306_handle_t display)
{
    if (!menu_active || current_menu >= MENU_MAX) {
        ssd1306_show_string(display, 20, 28, "Menu Inactive", 16, 1);
        return;
    }
    
    const menu_definition_t *menu = &menus[current_menu];
    if (menu->items == NULL) {
        ssd1306_show_string(display, 20, 28, "Invalid Menu", 16, 1);
        return;
    }
    
    // Display menu title based on current menu
    const char *title = "Main Menu";
    switch (current_menu) {
        case MENU_MAIN: title = "Main Menu"; break;
        case MENU_DISPLAY: title = "Display"; break;
        case MENU_NETWORK: title = "Network"; break;
        case MENU_SYSTEM: title = "System"; break;
        case MENU_ABOUT: title = "About"; break;
        default: title = "Menu"; break;
    }
    
    // Draw title with underline
    ssd1306_show_string(display, 0, 0, title, 16, 1);
    ssd1306_draw_line(display, 0, 15, 127, 15, 1);
    
    // Display menu items
    for (int i = 0; i < ITEMS_PER_PAGE && (i + scroll_offset) < menu->item_count; i++) {
        int item_index = i + scroll_offset;
        int y_pos = 18 + i * 12;
        
        // Highlight selected item
        if (item_index == current_selection) {
            // Draw selection rectangle
            ssd1306_draw_rectangle(display, 0, y_pos - 1, 127, 10, 1);
            // Invert text by drawing black text on white background
            for (int x = 1; x < 126; x++) {
                for (int y = y_pos; y < y_pos + 8; y++) {
                    ssd1306_draw_point(display, x, y, 1);
                }
            }
            ssd1306_show_string(display, 2, y_pos, menu->items[item_index].title, 16, 0);
        } else {
            ssd1306_show_string(display, 2, y_pos, menu->items[item_index].title, 16, 1);
        }
    }
    
    // Draw scroll indicators
    if (scroll_offset > 0) {
        ssd1306_show_string(display, 118, 18, "^", 16, 1);
    }
    if (scroll_offset + ITEMS_PER_PAGE < menu->item_count) {
        ssd1306_show_string(display, 118, 54, "v", 16, 1);
    }
    
    // Draw navigation hint
    char nav_hint[32];
    snprintf(nav_hint, sizeof(nav_hint), "%d/%d", current_selection + 1, menu->item_count);
    ssd1306_show_string(display, 90, 0, nav_hint, 16, 1);
}

void menu_system_navigate_up(void)
{
    if (!menu_active) return;
    
    const menu_definition_t *menu = &menus[current_menu];
    if (menu->items == NULL) return;
    
    if (current_selection > 0) {
        current_selection--;
        
        // Adjust scroll offset
        if (current_selection < scroll_offset) {
            scroll_offset = current_selection;
        }
    }
}

void menu_system_navigate_down(void)
{
    if (!menu_active) return;
    
    const menu_definition_t *menu = &menus[current_menu];
    if (menu->items == NULL) return;
    
    if (current_selection < menu->item_count - 1) {
        current_selection++;
        
        // Adjust scroll offset
        if (current_selection >= scroll_offset + ITEMS_PER_PAGE) {
            scroll_offset = current_selection - ITEMS_PER_PAGE + 1;
        }
    }
}

void menu_system_select(void)
{
    if (!menu_active) return;
    
    const menu_definition_t *menu = &menus[current_menu];
    if (menu->items == NULL || current_selection >= menu->item_count) return;
    
    const menu_item_t *item = &menu->items[current_selection];
    
    // Execute action if available
    if (item->action) {
        item->action();
    }
    
    // Navigate to submenu if available
    if (item->has_submenu) {
        current_menu = item->submenu;
        current_selection = 0;
        scroll_offset = 0;
    }
}

void menu_system_back(void)
{
    if (!menu_active) return;
    
    // Always go back to main menu for simplicity
    // In a more complex system, you'd maintain a navigation stack
    current_menu = MENU_MAIN;
    current_selection = 0;
    scroll_offset = 0;
}

bool menu_system_is_active(void)
{
    return menu_active;
}

void menu_system_activate(bool active)
{
    menu_active = active;
    if (active) {
        current_menu = MENU_MAIN;
        current_selection = 0;
        scroll_offset = 0;
    }
}

// Menu action implementations
static void action_brightness_up(void)
{
    ESP_LOGI(TAG, "Brightness up action");
    // In a real implementation, you would adjust display brightness
    // For SSD1306, you can change contrast
}

static void action_brightness_down(void)
{
    ESP_LOGI(TAG, "Brightness down action");
    // Adjust brightness/contrast down
}

static void action_wifi_scan(void)
{
    ESP_LOGI(TAG, "WiFi scan action");
    // Trigger WiFi scan
    esp_wifi_scan_start(NULL, false);
}

static void action_wifi_disconnect(void)
{
    ESP_LOGI(TAG, "WiFi disconnect action");
    esp_wifi_disconnect();
}

static void action_system_info(void)
{
    ESP_LOGI(TAG, "=== System Information ===");
    ESP_LOGI(TAG, "Free heap: %d bytes", esp_get_free_heap_size());
    ESP_LOGI(TAG, "Minimum free heap: %d bytes", esp_get_minimum_free_heap_size());
    
    // Get uptime
    uint32_t uptime_ms = xTaskGetTickCount() * portTICK_PERIOD_MS;
    uint32_t uptime_sec = uptime_ms / 1000;
    uint32_t hours = uptime_sec / 3600;
    uint32_t minutes = (uptime_sec % 3600) / 60;
    uint32_t seconds = uptime_sec % 60;
    
    ESP_LOGI(TAG, "Uptime: %02d:%02d:%02d", hours, minutes, seconds);
    ESP_LOGI(TAG, "Tasks running: %d", uxTaskGetNumberOfTasks());
}

static void action_factory_reset(void)
{
    ESP_LOGW(TAG, "Factory reset requested");
    // In a real implementation, you'd show a confirmation dialog
    nvs_flash_erase();
    esp_restart();
}

static void action_reboot(void)
{
    ESP_LOGI(TAG, "System reboot requested");
    esp_restart();
}
