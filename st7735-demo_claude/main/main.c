// main/main.c
#include <stdio.h>
#include <time.h>
#include <sys/time.h>
#include <math.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "st7735.h"

static const char *TAG = "MAIN";

// Pin definitions for ESP32-C3 Super Mini
#define PIN_MOSI  6
#define PIN_SCK   4
#define PIN_CS    7
#define PIN_DC    1
#define PIN_RESET 0

static st7735_handle_t display;

void text_examples(st7735_handle_t *display)
{
    // Example 1: Basic text display
    st7735_fill_screen(display, ST7735_BLACK);
    st7735_draw_string(display, 10, 10, "Hello ESP32!", ST7735_WHITE, ST7735_BLACK, 1);
    st7735_draw_string(display, 10, 25, "ST7735 Display", ST7735_CYAN, ST7735_BLACK, 1);
    vTaskDelay(pdMS_TO_TICKS(2000));

    // Example 2: Different text sizes
    st7735_fill_screen(display, ST7735_BLACK);
    st7735_draw_string(display, 5, 5, "Small", ST7735_WHITE, ST7735_BLACK, 1);
    st7735_draw_string(display, 5, 25, "Medium", ST7735_GREEN, ST7735_BLACK, 2);
    st7735_draw_string(display, 5, 65, "Big!", ST7735_RED, ST7735_BLACK, 3);
    vTaskDelay(pdMS_TO_TICKS(2000));

    // Example 3: Colored backgrounds
    st7735_fill_screen(display, ST7735_BLACK);
    st7735_draw_string(display, 5, 10, "White on Red", ST7735_WHITE, ST7735_RED, 1);
    st7735_draw_string(display, 5, 30, "Black on Yellow", ST7735_BLACK, ST7735_YELLOW, 1);
    st7735_draw_string(display, 5, 50, "Blue on White", ST7735_BLUE, ST7735_WHITE, 1);
    vTaskDelay(pdMS_TO_TICKS(2000));

    // Example 4: Printf-style formatting
    st7735_fill_screen(display, ST7735_BLACK);
    st7735_set_cursor(display, 5, 5);
    st7735_printf(display, ST7735_WHITE, ST7735_BLACK, 1, "Temperature: %.1fC", 23.5f);

    st7735_set_cursor(display, 5, 20);
    st7735_printf(display, ST7735_YELLOW, ST7735_BLACK, 1, "Humidity: %d%%", 65);

    st7735_set_cursor(display, 5, 35);
    st7735_printf(display, ST7735_CYAN, ST7735_BLACK, 1, "Count: %04d", 1234);

    st7735_set_cursor(display, 5, 50);
    st7735_printf(display, ST7735_GREEN, ST7735_BLACK, 1, "Voltage: %dmV", 3300);
    vTaskDelay(pdMS_TO_TICKS(3000));
}

void display_status_screen(st7735_handle_t *display)
{
    st7735_fill_screen(display, ST7735_BLACK);

    // Title bar
    st7735_fill_rect(display, 0, 0, 128, 15, ST7735_BLUE);
    st7735_draw_string(display, 30, 3, "SYSTEM STATUS", ST7735_WHITE, ST7735_BLUE, 1);

    // System info
    st7735_set_cursor(display, 5, 25);
    st7735_printf(display, ST7735_WHITE, ST7735_BLACK, 1, "CPU: ESP32-C3");

    st7735_set_cursor(display, 5, 40);
    st7735_printf(display, ST7735_GREEN, ST7735_BLACK, 1, "Status: RUNNING");

    st7735_set_cursor(display, 5, 55);
    st7735_printf(display, ST7735_YELLOW, ST7735_BLACK, 1, "Uptime: 15:32:45");

    st7735_set_cursor(display, 5, 70);
    st7735_printf(display, ST7735_CYAN, ST7735_BLACK, 1, "Free RAM: 245KB");

    // Separator line
    st7735_draw_hline(display, 5, 85, 118, ST7735_WHITE);

    // Network status
    st7735_set_cursor(display, 5, 95);
    st7735_printf(display, ST7735_WHITE, ST7735_BLACK, 1, "WiFi: Connected");

    st7735_set_cursor(display, 5, 110);
    st7735_printf(display, ST7735_MAGENTA, ST7735_BLACK, 1, "IP: 192.168.1.100");

    // Bottom status bar
    st7735_fill_rect(display, 0, 145, 128, 15, ST7735_GREEN);
    st7735_draw_string(display, 40, 148, "ALL SYSTEMS OK", ST7735_BLACK, ST7735_GREEN, 1);
    vTaskDelay(pdMS_TO_TICKS(5000));
}

void display_sensor_dashboard(st7735_handle_t *display, float temp, int humidity, int pressure)
{
    st7735_fill_screen(display, ST7735_BLACK);

    // Title
    st7735_draw_string(display, 25, 5, "SENSOR DATA", ST7735_WHITE, ST7735_BLACK, 2);

    // Temperature section
    st7735_fill_rect(display, 5, 30, 118, 25, ST7735_RED);
    st7735_draw_string(display, 10, 35, "TEMPERATURE", ST7735_WHITE, ST7735_RED, 1);
    st7735_set_cursor(display, 10, 45);
    st7735_printf(display, ST7735_WHITE, ST7735_RED, 1, "%.1f C", temp);

    // Humidity section
    st7735_fill_rect(display, 5, 60, 118, 25, ST7735_BLUE);
    st7735_draw_string(display, 10, 65, "HUMIDITY", ST7735_WHITE, ST7735_BLUE, 1);
    st7735_set_cursor(display, 10, 75);
    st7735_printf(display, ST7735_WHITE, ST7735_BLUE, 1, "%d %%", humidity);

    // Pressure section
    st7735_fill_rect(display, 5, 90, 118, 25, ST7735_GREEN);
    st7735_draw_string(display, 10, 95, "PRESSURE", ST7735_WHITE, ST7735_GREEN, 1);
    st7735_set_cursor(display, 10, 105);
    st7735_printf(display, ST7735_WHITE, ST7735_GREEN, 1, "%d hPa", pressure);

    // Timestamp
    st7735_set_cursor(display, 20, 130);
    st7735_printf(display, ST7735_YELLOW, ST7735_BLACK, 1, "Updated: 14:30:15");
    vTaskDelay(pdMS_TO_TICKS(5000));
}

void display_digital_clock(st7735_handle_t *display)
{
    st7735_fill_screen(display, ST7735_BLACK);

    // Get current time (you would replace this with actual RTC reading)
    time_t now;
    struct tm timeinfo;
    time(&now);
    localtime_r(&now, &timeinfo);

    // Digital clock display
    st7735_set_cursor(display, 15, 40);
    st7735_printf(display, ST7735_CYAN, ST7735_BLACK, 3, "%02d:%02d", 
                  timeinfo.tm_hour, timeinfo.tm_min);

    st7735_set_cursor(display, 45, 80);
    st7735_printf(display, ST7735_WHITE, ST7735_BLACK, 2, "%02d", timeinfo.tm_sec);

    // Date
    st7735_set_cursor(display, 20, 110);
    st7735_printf(display, ST7735_YELLOW, ST7735_BLACK, 1, "%04d-%02d-%02d", 
                  timeinfo.tm_year + 1900, timeinfo.tm_mon + 1, timeinfo.tm_mday);

    // Day of week
    const char* days[] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
    st7735_set_cursor(display, 45, 130);
    st7735_printf(display, ST7735_GREEN, ST7735_BLACK, 1, "%s", days[timeinfo.tm_wday]);
    vTaskDelay(pdMS_TO_TICKS(5000));
}

void display_progress_bars(st7735_handle_t *display)
{
    st7735_fill_screen(display, ST7735_BLACK);

    st7735_draw_string(display, 30, 5, "PROGRESS", ST7735_WHITE, ST7735_BLACK, 1);

    // CPU Usage (75%)
    st7735_draw_string(display, 5, 25, "CPU:", ST7735_WHITE, ST7735_BLACK, 1);
    st7735_draw_rect(display, 35, 25, 82, 10, ST7735_WHITE);
    st7735_fill_rect(display, 36, 26, 60, 8, ST7735_RED);  // 75% of 80 pixels
    st7735_set_cursor(display, 120, 25);
    st7735_printf(display, ST7735_WHITE, ST7735_BLACK, 1, "75%%");

    // Memory Usage (45%)
    st7735_draw_string(display, 5, 45, "RAM:", ST7735_WHITE, ST7735_BLACK, 1);
    st7735_draw_rect(display, 35, 45, 82, 10, ST7735_WHITE);
    st7735_fill_rect(display, 36, 46, 36, 8, ST7735_YELLOW);  // 45% of 80 pixels
    st7735_set_cursor(display, 120, 45);
    st7735_printf(display, ST7735_WHITE, ST7735_BLACK, 1, "45%%");

    // Storage Usage (20%)
    st7735_draw_string(display, 5, 65, "DISK:", ST7735_WHITE, ST7735_BLACK, 1);
    st7735_draw_rect(display, 35, 65, 82, 10, ST7735_WHITE);
    st7735_fill_rect(display, 36, 66, 16, 8, ST7735_GREEN);  // 20% of 80 pixels
    st7735_set_cursor(display, 120, 65);
    st7735_printf(display, ST7735_WHITE, ST7735_BLACK, 1, "20%%");

    // Network Usage (90%)
    st7735_draw_string(display, 5, 85, "NET:", ST7735_WHITE, ST7735_BLACK, 1);
    st7735_draw_rect(display, 35, 85, 82, 10, ST7735_WHITE);
    st7735_fill_rect(display, 36, 86, 72, 8, ST7735_BLUE);  // 90% of 80 pixels
    st7735_set_cursor(display, 120, 85);
    st7735_printf(display, ST7735_WHITE, ST7735_BLACK, 1, "90%%");
    vTaskDelay(pdMS_TO_TICKS(5000));
}

void display_menu_system(st7735_handle_t *display, int selected_index)
{
    const char* menu_items[] = {
        "Settings",
        "Display",
        "Network", 
        "System",
        "About",
        "Exit"
    };

    st7735_fill_screen(display, ST7735_BLACK);

    // Title
    st7735_fill_rect(display, 0, 0, 128, 20, ST7735_BLUE);
    st7735_draw_string(display, 45, 5, "MAIN MENU", ST7735_WHITE, ST7735_BLUE, 1);

    // Menu items
    for (int i = 0; i < 6; i++) {
        uint16_t text_color = (i == selected_index) ? ST7735_BLACK : ST7735_WHITE;
        uint16_t bg_color = (i == selected_index) ? ST7735_GREEN : ST7735_BLACK;

        if (i == selected_index) {
            st7735_fill_rect(display, 5, 25 + i * 15, 118, 12, ST7735_GREEN);
            st7735_draw_string(display, 2, 25 + i * 15, ">", ST7735_WHITE, ST7735_BLACK, 1);
        }

        st7735_draw_string(display, 15, 25 + i * 15, menu_items[i], text_color, bg_color, 1);
    }

    // Instructions
    st7735_draw_string(display, 10, 140, "UP/DOWN: Navigate", ST7735_YELLOW, ST7735_BLACK, 1);
    st7735_draw_string(display, 10, 150, "ENTER: Select", ST7735_YELLOW, ST7735_BLACK, 1);
    vTaskDelay(pdMS_TO_TICKS(5000));
}

void graphics_demo_task(void *param)
{
    ESP_LOGI(TAG, "Starting graphics demo");

    while (1) {
        // Fill screen with different colors
        ESP_LOGI(TAG, "Filling screen with colors");
        st7735_fill_screen(&display, ST7735_RED);
        vTaskDelay(pdMS_TO_TICKS(1000));

        st7735_fill_screen(&display, ST7735_GREEN);
        vTaskDelay(pdMS_TO_TICKS(1000));

        st7735_fill_screen(&display, ST7735_BLUE);
        vTaskDelay(pdMS_TO_TICKS(1000));

        // Clear to black
        st7735_fill_screen(&display, ST7735_BLACK);

        // Example usage of text functions for ST7735 display
        text_examples(&display);
        display_status_screen(&display);
        display_sensor_dashboard(&display, 32.3, 65, 102);
        display_digital_clock(&display);
        display_progress_bars(&display);
        display_menu_system(&display, 1);

        // Draw rectangles
        ESP_LOGI(TAG, "Drawing rectangles");
        st7735_fill_screen(&display, ST7735_BLACK);
        st7735_fill_rect(&display, 10, 10, 50, 30, ST7735_RED);
        st7735_fill_rect(&display, 70, 10, 50, 30, ST7735_GREEN);
        st7735_fill_rect(&display, 10, 50, 50, 30, ST7735_BLUE);
        st7735_fill_rect(&display, 70, 50, 50, 30, ST7735_YELLOW);
        vTaskDelay(pdMS_TO_TICKS(2000));

        // Draw outline rectangles
        ESP_LOGI(TAG, "Drawing outline rectangles");
        st7735_fill_screen(&display, ST7735_BLACK);
        st7735_draw_rect(&display, 10, 10, 108, 60, ST7735_WHITE);
        st7735_draw_rect(&display, 15, 15, 98, 50, ST7735_RED);
        st7735_draw_rect(&display, 20, 20, 88, 40, ST7735_GREEN);
        st7735_draw_rect(&display, 25, 25, 78, 30, ST7735_BLUE);
        vTaskDelay(pdMS_TO_TICKS(2000));

        // Draw circles
        ESP_LOGI(TAG, "Drawing circles");
        st7735_fill_screen(&display, ST7735_BLACK);
        st7735_draw_circle(&display, 64, 80, 50, ST7735_WHITE);
        st7735_draw_circle(&display, 64, 80, 40, ST7735_RED);
        st7735_draw_circle(&display, 64, 80, 30, ST7735_GREEN);
        st7735_draw_circle(&display, 64, 80, 20, ST7735_BLUE);
        vTaskDelay(pdMS_TO_TICKS(2000));

        // Draw filled circles
        ESP_LOGI(TAG, "Drawing filled circles");
        st7735_fill_screen(&display, ST7735_BLACK);
        st7735_fill_circle(&display, 32, 60, 25, ST7735_RED);
        st7735_fill_circle(&display, 96, 60, 25, ST7735_GREEN);
        st7735_fill_circle(&display, 64, 100, 25, ST7735_BLUE);
        vTaskDelay(pdMS_TO_TICKS(2000));

        // Draw lines pattern
        ESP_LOGI(TAG, "Drawing lines pattern");
        st7735_fill_screen(&display, ST7735_BLACK);

        // Horizontal lines
        for (int y = 10; y < 160; y += 10) {
            st7735_draw_hline(&display, 0, y, 128, st7735_rgb_to_565(y, 255-y, 128));
        }
        vTaskDelay(pdMS_TO_TICKS(1500));

        // Vertical lines
        st7735_fill_screen(&display, ST7735_BLACK);
        for (int x = 10; x < 128; x += 10) {
            st7735_draw_vline(&display, x, 0, 160, st7735_rgb_to_565(x*2, 128, 255-x*2));
        }
        vTaskDelay(pdMS_TO_TICKS(1500));

        // Animated bouncing ball
        ESP_LOGI(TAG, "Animated bouncing ball");
        int ball_x = 20, ball_y = 20;
        int vel_x = 3, vel_y = 2;
        int ball_radius = 8;

        for (int i = 0; i < 200; i++) {
            // Clear previous ball position
            st7735_fill_screen(&display, ST7735_BLACK);

            // Draw ball
            st7735_fill_circle(&display, ball_x, ball_y, ball_radius, ST7735_CYAN);

            // Update position
            ball_x += vel_x;
            ball_y += vel_y;

            // Bounce off walls
            if (ball_x - ball_radius <= 0 || ball_x + ball_radius >= 128) {
                vel_x = -vel_x;
            }
            if (ball_y - ball_radius <= 0 || ball_y + ball_radius >= 160) {
                vel_y = -vel_y;
            }

            // Keep ball in bounds
            if (ball_x - ball_radius < 0) ball_x = ball_radius;
            if (ball_x + ball_radius > 128) ball_x = 128 - ball_radius;
            if (ball_y - ball_radius < 0) ball_y = ball_radius;
            if (ball_y + ball_radius > 160) ball_y = 160 - ball_radius;

            vTaskDelay(pdMS_TO_TICKS(50));
        }

        // Gradient effect
        ESP_LOGI(TAG, "Drawing gradient effect");
        for (int y = 0; y < 160; y++) {
            uint16_t color = st7735_rgb_to_565(y*255/160, (160-y)*255/160, 128);
            st7735_draw_hline(&display, 0, y, 128, color);
        }
        vTaskDelay(pdMS_TO_TICKS(2000));

        // Checkered pattern
        ESP_LOGI(TAG, "Drawing checkered pattern");
        st7735_fill_screen(&display, ST7735_BLACK);
        int square_size = 16;
        for (int y = 0; y < 160; y += square_size) {
            for (int x = 0; x < 128; x += square_size) {
                if (((x/square_size) + (y/square_size)) % 2 == 0) {
                    st7735_fill_rect(&display, x, y, square_size, square_size, ST7735_WHITE);
                }
            }
        }
        vTaskDelay(pdMS_TO_TICKS(2000));

        // Spiral pattern
        ESP_LOGI(TAG, "Drawing spiral pattern");
        st7735_fill_screen(&display, ST7735_BLACK);
        int center_x = 64, center_y = 80;

        for (int i = 0; i < 200; i++) {
            float angle = i * 0.2f;
            float radius = i * 0.3f;
            int x = center_x + (int)(radius * cosf(angle));
            int y = center_y + (int)(radius * sinf(angle));

            if (x >= 0 && x < 128 && y >= 0 && y < 160) {
                uint16_t color = st7735_rgb_to_565((i*3) % 256, (i*5) % 256, (i*7) % 256);
                st7735_set_pixel(&display, x, y, color);
            }
        }
        vTaskDelay(pdMS_TO_TICKS(3000));

        ESP_LOGI(TAG, "Demo cycle complete, restarting...");
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void app_main(void)
{
    ESP_LOGI(TAG, "ST7735 Display Demo Starting");

    // Configure display
    st7735_config_t config = {
        .pin_mosi = PIN_MOSI,
        .pin_sck = PIN_SCK,
        .pin_cs = PIN_CS,
        .pin_dc = PIN_DC,
        .pin_reset = PIN_RESET,
        .spi_host = SPI2_HOST,
    };

    // Initialize display
    esp_err_t ret = st7735_init(&display, &config);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to initialize ST7735 display: %s", esp_err_to_name(ret));
        return;
    }

    ESP_LOGI(TAG, "Display initialized successfully");

    // Start graphics demo
    xTaskCreate(graphics_demo_task, "graphics_demo", 4096, NULL, 5, NULL);
}
