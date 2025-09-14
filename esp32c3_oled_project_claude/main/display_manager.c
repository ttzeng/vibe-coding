#include <stdio.h>
#include <string.h>
#include <time.h>
#include "esp_log.h"
#include "esp_system.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "display_manager.h"
#include "animations.h"
#include "menu_system.h"
#include "utils.h"
#include <math.h>

static const char *TAG = "DISPLAY_MGR";

struct display_manager_t {
    ssd1306_handle_t display;
    display_mode_t current_mode;
    uint32_t frame_count;
    uint32_t last_update;
    animation_type_t current_animation;
};

static system_status_t g_system_status = {0};

// Mode display functions
static void display_clock_mode(display_manager_handle_t manager);
static void display_system_info_mode(display_manager_handle_t manager);
static void display_sensor_data_mode(display_manager_handle_t manager);
static void display_network_info_mode(display_manager_handle_t manager);
static void display_animations_mode(display_manager_handle_t manager);
static void display_menu_mode(display_manager_handle_t manager);

display_manager_handle_t display_manager_create(ssd1306_handle_t display)
{
    if (display == NULL) {
        ESP_LOGE(TAG, "Display handle is NULL");
        return NULL;
    }
    
    display_manager_handle_t manager = malloc(sizeof(struct display_manager_t));
    if (manager == NULL) {
        ESP_LOGE(TAG, "Failed to allocate memory for display manager");
        return NULL;
    }
    
    manager->display = display;
    manager->current_mode = DISPLAY_MODE_CLOCK;
    manager->frame_count = 0;
    manager->last_update = 0;
    manager->current_animation = ANIM_BOUNCING_BALL;
    
    ESP_LOGI(TAG, "Display manager created successfully");
    return manager;
}

void display_manager_delete(display_manager_handle_t manager)
{
    if (manager) {
        free(manager);
        ESP_LOGI(TAG, "Display manager deleted");
    }
}

esp_err_t display_manager_set_mode(display_manager_handle_t manager, display_mode_t mode)
{
    if (manager == NULL || mode >= DISPLAY_MODE_MAX) {
        return ESP_ERR_INVALID_ARG;
    }
    
    manager->current_mode = mode;
    manager->frame_count = 0;
    
    // Reset animation when entering animation mode
    if (mode == DISPLAY_MODE_ANIMATIONS) {
        animations_reset();
    }
    
    return ESP_OK;
}

esp_err_t display_manager_update(display_manager_handle_t manager)
{
    if (manager == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    
    uint32_t now = xTaskGetTickCount() * portTICK_PERIOD_MS;
    manager->last_update = now;
    manager->frame_count++;
    
    // Update system status
    g_system_status.free_heap = esp_get_free_heap_size();
    g_system_status.uptime_seconds = now / 1000;
    time(&g_system_status.current_time);
    
    // Clear screen
    ssd1306_clear_screen(manager->display, 0x00);
    
    // Display current mode
    switch (manager->current_mode) {
        case DISPLAY_MODE_CLOCK:
            display_clock_mode(manager);
            break;
        case DISPLAY_MODE_SYSTEM_INFO:
            display_system_info_mode(manager);
            break;
        case DISPLAY_MODE_SENSOR_DATA:
            display_sensor_data_mode(manager);
            break;
        case DISPLAY_MODE_NETWORK_INFO:
            display_network_info_mode(manager);
            break;
        case DISPLAY_MODE_ANIMATIONS:
            display_animations_mode(manager);
            break;
        case DISPLAY_MODE_MENU:
            display_menu_mode(manager);
            break;
        default:
            ssd1306_show_string(manager->display, 0, 0, "Unknown Mode", 16, 1);
            break;
    }
    
    // Refresh display
    ssd1306_refresh_gram(manager->display);
    
    return ESP_OK;
}

esp_err_t display_manager_show_startup(display_manager_handle_t manager)
{
    if (manager == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    
    ssd1306_clear_screen(manager->display, 0x00);
    
    // Draw startup screen
    ssd1306_show_string(manager->display, 0, 0, APP_NAME, 16, 1);
    ssd1306_show_string(manager->display, 0, 16, "Version: " APP_VERSION, 16, 1);
    ssd1306_show_string(manager->display, 0, 32, "Initializing...", 16, 1);
    
    // Draw progress bar
    ssd1306_draw_rectangle(manager->display, 0, 50, 127, 10, 1);
    
    for (int i = 0; i < 3; i++) {
        int progress_width = (i + 1) * 40;
        for (int x = 1; x < progress_width && x < 126; x++) {
            for (int y = 51; y < 59; y++) {
                ssd1306_draw_point(manager->display, x, y, 1);
            }
        }
        ssd1306_refresh_gram(manager->display);
        vTaskDelay(pdMS_TO_TICKS(500));
    }
    
    return ESP_OK;
}

void display_manager_update_system_status(system_status_t *status)
{
    if (status) {
        memcpy(&g_system_status, status, sizeof(system_status_t));
    }
}

// Mode display implementations
static void display_clock_mode(display_manager_handle_t manager)
{
    char time_str[32];
    char date_str[32];
    struct tm timeinfo;
    
    if (g_system_status.current_time > 0) {
        localtime_r(&g_system_status.current_time, &timeinfo);
        strftime(time_str, sizeof(time_str), "%H:%M:%S", &timeinfo);
        strftime(date_str, sizeof(date_str), "%Y-%m-%d", &timeinfo);
    } else {
        strcpy(time_str, "--:--:--");
        strcpy(date_str, "----/--/--");
    }
    
    // Display large time
    ssd1306_show_string(manager->display, 0, 8, time_str, 16, 1);
    ssd1306_show_string(manager->display, 0, 28, date_str, 16, 1);
    
    // Show uptime
    char uptime_str[32];
    uint32_t hours = g_system_status.uptime_seconds / 3600;
    uint32_t minutes = (g_system_status.uptime_seconds % 3600) / 60;
    snprintf(uptime_str, sizeof(uptime_str), "Up: %ldh %ldm", hours, minutes);
    ssd1306_show_string(manager->display, 0, 48, uptime_str, 16, 1);
}

static void display_system_info_mode(display_manager_handle_t manager)
{
    char info_str[32];
    
    ssd1306_show_string(manager->display, 0, 0, "System Info", 16, 1);
    
    // Free heap
    snprintf(info_str, sizeof(info_str), "Heap: %ld KB", g_system_status.free_heap / 1024);
    ssd1306_show_string(manager->display, 0, 16, info_str, 16, 1);
    
    // CPU frequency - simplified version
    snprintf(info_str, sizeof(info_str), "CPU: %d MHz", 160); // Default ESP32-C3 frequency
    ssd1306_show_string(manager->display, 0, 32, info_str, 16, 1);
    
    // Frame rate
    uint32_t fps = 0;
    if (manager->last_update > 0) {
        fps = manager->frame_count * 1000 / manager->last_update;
        if (fps > 100) fps = 100; // Cap at reasonable value
    }
    snprintf(info_str, sizeof(info_str), "FPS: %ld", fps);
    ssd1306_show_string(manager->display, 0, 48, info_str, 16, 1);
}

static void display_sensor_data_mode(display_manager_handle_t manager)
{
    char sensor_str[32];
    
    ssd1306_show_string(manager->display, 0, 0, "Sensors", 16, 1);
    
    // Temperature (simulated)
    snprintf(sensor_str, sizeof(sensor_str), "Temp: %.1f C", g_system_status.temperature);
    ssd1306_show_string(manager->display, 0, 16, sensor_str, 16, 1);
    
    // Humidity (simulated)
    snprintf(sensor_str, sizeof(sensor_str), "Hum: %.1f %%", g_system_status.humidity);
    ssd1306_show_string(manager->display, 0, 32, sensor_str, 16, 1);
    
    // Some animation
    int x = 64 + 32 * sin(manager->frame_count * 0.1);
    ssd1306_draw_point(manager->display, x, 50, 1);
}

static void display_network_info_mode(display_manager_handle_t manager)
{
    char net_str[64];
    
    ssd1306_show_string(manager->display, 0, 0, "Network", 16, 1);
    
    if (g_system_status.wifi_connected) {
        snprintf(net_str, sizeof(net_str), "WiFi: %s", g_system_status.wifi_ssid);
        ssd1306_show_string(manager->display, 0, 16, net_str, 16, 1);
        
        snprintf(net_str, sizeof(net_str), "IP: %s", g_system_status.ip_address);
        ssd1306_show_string(manager->display, 0, 32, net_str, 16, 1);
        
        snprintf(net_str, sizeof(net_str), "RSSI: %d dBm", g_system_status.wifi_rssi);
        ssd1306_show_string(manager->display, 0, 48, net_str, 16, 1);
    } else {
        ssd1306_show_string(manager->display, 0, 16, "WiFi: Disconnected", 16, 1);
        ssd1306_show_string(manager->display, 0, 32, "Connecting...", 16, 1);
    }
}

static void display_animations_mode(display_manager_handle_t manager)
{
    // Cycle through animations every 5 seconds
    if (manager->frame_count % 50 == 0) {
        manager->current_animation = (manager->current_animation + 1) % ANIM_MAX;
        if (manager->current_animation == ANIM_NONE) {
            manager->current_animation = ANIM_BOUNCING_BALL;
        }
        animations_set_type(manager->current_animation);
    }
    
    animations_update(manager->display, manager->frame_count);
}

static void display_menu_mode(display_manager_handle_t manager)
{
    menu_system_display(manager->display);
}
