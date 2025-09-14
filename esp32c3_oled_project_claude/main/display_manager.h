#ifndef DISPLAY_MANAGER_H
#define DISPLAY_MANAGER_H

#include "ssd1306.h"
#include "app_config.h"

typedef struct display_manager_t* display_manager_handle_t;

typedef struct {
    float temperature;
    float humidity;
    uint32_t free_heap;
    uint32_t uptime_seconds;
    char wifi_ssid[32];
    int8_t wifi_rssi;
    bool wifi_connected;
    char ip_address[16];
    time_t current_time;
} system_status_t;

// Display Manager API
display_manager_handle_t display_manager_create(ssd1306_handle_t display);
void display_manager_delete(display_manager_handle_t manager);
esp_err_t display_manager_set_mode(display_manager_handle_t manager, display_mode_t mode);
esp_err_t display_manager_update(display_manager_handle_t manager);
esp_err_t display_manager_show_startup(display_manager_handle_t manager);

// Status update functions
void display_manager_update_system_status(system_status_t *status);

#endif // DISPLAY_MANAGER_H
