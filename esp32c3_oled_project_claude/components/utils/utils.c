#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include <math.h>
#include <time.h>
#include <ctype.h>
#include "utils.h"
#include "esp_system.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "ssd1306.h"

static const char *TAG = "UTILS";

// Math utilities
float utils_map_float(float x, float in_min, float in_max, float out_min, float out_max)
{
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

int utils_map_int(int x, int in_min, int in_max, int out_min, int out_max)
{
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

float utils_clamp_float(float value, float min_val, float max_val)
{
    if (value < min_val) return min_val;
    if (value > max_val) return max_val;
    return value;
}

int utils_clamp_int(int value, int min_val, int max_val)
{
    if (value < min_val) return min_val;
    if (value > max_val) return max_val;
    return value;
}

// String utilities
void utils_format_bytes(uint32_t bytes, char* buffer, size_t buffer_size)
{
    if (buffer == NULL || buffer_size == 0) return;
    
    const char* units[] = {"B", "KB", "MB", "GB"};
    int unit_index = 0;
    float size = (float)bytes;
    
    while (size >= 1024.0 && unit_index < 3) {
        size /= 1024.0;
        unit_index++;
    }
    
    if (unit_index == 0) {
        snprintf(buffer, buffer_size, "%d %s", (int)size, units[unit_index]);
    } else {
        snprintf(buffer, buffer_size, "%.1f %s", size, units[unit_index]);
    }
}

void utils_format_duration(uint32_t seconds, char* buffer, size_t buffer_size)
{
    if (buffer == NULL || buffer_size == 0) return;
    
    uint32_t days = seconds / 86400;
    uint32_t hours = (seconds % 86400) / 3600;
    uint32_t minutes = (seconds % 3600) / 60;
    uint32_t secs = seconds % 60;
    
    if (days > 0) {
        snprintf(buffer, buffer_size, "%ld %02ld:%02ld:%02ld", days, hours, minutes, secs);
    } else {
        snprintf(buffer, buffer_size, "%02ld:%02ld:%02ld", hours, minutes, secs);
    }
}

bool utils_string_ends_with(const char* str, const char* suffix)
{
    if (str == NULL || suffix == NULL) return false;
    
    size_t str_len = strlen(str);
    size_t suffix_len = strlen(suffix);
    
    if (suffix_len > str_len) return false;
    
    return strcmp(str + str_len - suffix_len, suffix) == 0;
}

void utils_string_trim(char* str)
{
    if (str == NULL) return;
    
    // Trim leading spaces
    char* start = str;
    while (isspace((int)*start)) start++;
    
    // Move string to the beginning
    if (start != str) {
        memmove(str, start, strlen(start) + 1);
    }
    
    // Trim trailing spaces
    char* end = str + strlen(str) - 1;
    while (end >= str && isspace((int)*end)) {
        *end = '\0';
        end--;
    }
}

// Time utilities
void utils_get_time_string(char* buffer, size_t buffer_size, const char* format)
{
    if (buffer == NULL || buffer_size == 0) return;
    
    time_t now;
    struct tm timeinfo;
    
    time(&now);
    localtime_r(&now, &timeinfo);
    
    if (format == NULL) {
        format = "%Y-%m-%d %H:%M:%S";
    }
    
    strftime(buffer, buffer_size, format, &timeinfo);
}

uint32_t utils_get_uptime_ms(void)
{
    return xTaskGetTickCount() * portTICK_PERIOD_MS;
}

uint32_t utils_get_uptime_seconds(void)
{
    return utils_get_uptime_ms() / 1000;
}

// System utilities
void utils_print_system_info(void)
{
    ESP_LOGI(TAG, "=== System Information ===");
    ESP_LOGI(TAG, "Free heap: %d bytes", esp_get_free_heap_size());
    ESP_LOGI(TAG, "Min free heap: %d bytes", esp_get_minimum_free_heap_size());
    //ESP_LOGI(TAG, "CPU frequency: %d MHz", (int)(esp_clk_cpu_freq() / 1000000));
    //ESP_LOGI(TAG, "Flash size: %d MB", spi_flash_get_chip_size() / (1024 * 1024));
    
    char uptime_str[32];
    utils_format_duration(utils_get_uptime_seconds(), uptime_str, sizeof(uptime_str));
    ESP_LOGI(TAG, "Uptime: %s", uptime_str);
}

void utils_print_task_info(void)
{
    ESP_LOGI(TAG, "=== Task Information ===");
    ESP_LOGI(TAG, "Number of tasks: %d", uxTaskGetNumberOfTasks());
    
    char* task_list = malloc(1024);
    if (task_list) {
        vTaskList(task_list);
        ESP_LOGI(TAG, "Task list:\n%s", task_list);
        free(task_list);
    }
}

float utils_get_cpu_usage(void)
{
    // Simple CPU usage estimation based on idle task
    // This is a rough approximation
    static uint32_t last_idle_count = 0;
    static uint32_t last_time = 0;
    
    uint32_t current_time = utils_get_uptime_ms();
    uint32_t current_idle_count = 0; // Would need to implement idle task monitoring
    
    if (last_time == 0) {
        last_time = current_time;
        last_idle_count = current_idle_count;
        return 0.0;
    }
    
    uint32_t time_diff = current_time - last_time;
    uint32_t idle_diff = current_idle_count - last_idle_count;
    
    float cpu_usage = 100.0 - ((float)idle_diff / time_diff * 100.0);
    
    last_time = current_time;
    last_idle_count = current_idle_count;
    
    return utils_clamp_float(cpu_usage, 0.0, 100.0);
}

// Display utilities
void utils_draw_progress_bar(void* display, int x, int y, int width, int height, 
                           float progress, bool border)
{
    ssd1306_handle_t disp = (ssd1306_handle_t)display;
    if (disp == NULL) return;
    
    progress = utils_clamp_float(progress, 0.0, 1.0);
    
    if (border) {
        ssd1306_draw_rectangle(disp, x, y, width, height, 1);
        x += 1;
        y += 1;
        width -= 2;
        height -= 2;
    }
    
    int fill_width = (int)(width * progress);
    
    for (int px = x; px < x + width; px++) {
        for (int py = y; py < y + height; py++) {
            int fill = (px < x + fill_width) ? 1 : 0;
            ssd1306_draw_point(disp, px, py, fill);
        }
    }
}

void utils_draw_signal_strength(void* display, int x, int y, int8_t rssi)
{
    ssd1306_handle_t disp = (ssd1306_handle_t)display;
    if (disp == NULL) return;
    
    // Convert RSSI to signal strength (0-4 bars)
    int bars = 0;
    if (rssi > -50) bars = 4;
    else if (rssi > -60) bars = 3;
    else if (rssi > -70) bars = 2;
    else if (rssi > -80) bars = 1;
    
    // Draw signal bars
    for (int i = 0; i < 4; i++) {
        int bar_height = 2 + i * 2;
        int bar_x = x + i * 3;
        int bar_y = y + 8 - bar_height;
        
        if (i < bars) {
            // Filled bar
            for (int px = bar_x; px < bar_x + 2; px++) {
                for (int py = bar_y; py < y + 8; py++) {
                    ssd1306_draw_point(disp, px, py, 1);
                }
            }
        } else {
            // Empty bar outline
            ssd1306_draw_rectangle(disp, bar_x, bar_y, 2, bar_height, 1);
        }
    }
}

void utils_draw_battery_icon(void* display, int x, int y, float percentage)
{
    ssd1306_handle_t disp = (ssd1306_handle_t)display;
    if (disp == NULL) return;
    
    percentage = utils_clamp_float(percentage, 0.0, 100.0);
    
    // Battery outline (12x6 pixels)
    ssd1306_draw_rectangle(disp, x, y, 11, 6, 1);
    ssd1306_draw_rectangle(disp, x + 11, y + 1, 2, 4, 1);
    
    // Battery fill
    int fill_width = (int)((percentage / 100.0) * 9);
    for (int px = x + 1; px < x + 1 + fill_width; px++) {
        for (int py = y + 1; py < y + 5; py++) {
            ssd1306_draw_point(disp, px, py, 1);
        }
    }
}
