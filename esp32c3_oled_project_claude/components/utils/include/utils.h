#ifndef UTILS_H
#define UTILS_H

#include <stdint.h>
#include <stdbool.h>

// Math utilities
float utils_map_float(float x, float in_min, float in_max, float out_min, float out_max);
int utils_map_int(int x, int in_min, int in_max, int out_min, int out_max);
float utils_clamp_float(float value, float min_val, float max_val);
int utils_clamp_int(int value, int min_val, int max_val);

// String utilities
void utils_format_bytes(uint32_t bytes, char* buffer, size_t buffer_size);
void utils_format_duration(uint32_t seconds, char* buffer, size_t buffer_size);
bool utils_string_ends_with(const char* str, const char* suffix);
void utils_string_trim(char* str);

// Time utilities
void utils_get_time_string(char* buffer, size_t buffer_size, const char* format);
uint32_t utils_get_uptime_ms(void);
uint32_t utils_get_uptime_seconds(void);

// System utilities
void utils_print_system_info(void);
void utils_print_task_info(void);
float utils_get_cpu_usage(void);

// Display utilities
void utils_draw_progress_bar(void* display, int x, int y, int width, int height, 
                           float progress, bool border);
void utils_draw_signal_strength(void* display, int x, int y, int8_t rssi);
void utils_draw_battery_icon(void* display, int x, int y, float percentage);

#endif // UTILS_H
