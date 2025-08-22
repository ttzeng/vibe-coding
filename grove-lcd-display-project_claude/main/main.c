/**
 * @file main.c
 * @brief Example usage of Grove LCD RGB component
 */

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "grove_lcd_rgb.h"

static const char *TAG = "main";

void app_main(void)
{
    ESP_LOGI(TAG, "Starting Grove LCD RGB Example");
    
    // Initialize Grove LCD RGB with default configuration
    grove_lcd_rgb_config_t lcd_config = GROVE_LCD_RGB_CONFIG_DEFAULT();
    grove_lcd_rgb_handle_t lcd_handle;
    
    esp_err_t ret = grove_lcd_rgb_init(&lcd_config, &lcd_handle);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to initialize Grove LCD RGB: %s", esp_err_to_name(ret));
        return;
    }
    
    // Wait for LCD to be fully ready
    ret = grove_lcd_rgb_ready(lcd_handle);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to ready Grove LCD RGB: %s", esp_err_to_name(ret));
        return;
    }
    
    // Set initial backlight color to blue
    grove_lcd_rgb_color_t blue = GROVE_LCD_COLOR_BLUE;
    grove_lcd_rgb_set_color(lcd_handle, blue);
    
    // Clear display and show welcome message
    grove_lcd_rgb_clear(lcd_handle);
    grove_lcd_rgb_set_cursor(lcd_handle, 0, 0);
    grove_lcd_rgb_print(lcd_handle, "Hello World");
    grove_lcd_rgb_set_cursor(lcd_handle, 0, 1);
    grove_lcd_rgb_print(lcd_handle, "ESP32-C3");
    
    ESP_LOGI(TAG, "Initial display set up");
    
    // Color cycling demo
    grove_lcd_rgb_color_t colors[] = {
        GROVE_LCD_COLOR_RED,
        GROVE_LCD_COLOR_GREEN,
        GROVE_LCD_COLOR_BLUE,
        GROVE_LCD_COLOR_YELLOW,
        GROVE_LCD_COLOR_MAGENTA,
        GROVE_LCD_COLOR_CYAN,
        GROVE_LCD_COLOR_WHITE,
    };
    
    const char *color_names[] = {
        "Red", "Green", "Blue", "Yellow", "Magenta", "Cyan", "White"
    };
    
    int color_index = 0;
    int counter = 0;
    
    while (1) {
        vTaskDelay(pdMS_TO_TICKS(2000));  // Wait 2 seconds
        
        // Change backlight color
        color_index = (color_index + 1) % 7;
        grove_lcd_rgb_set_color(lcd_handle, colors[color_index]);
        
        // Update display with counter and current color
        grove_lcd_rgb_clear(lcd_handle);
        grove_lcd_rgb_set_cursor(lcd_handle, 0, 0);
        grove_lcd_rgb_printf(lcd_handle, "Count: %d", ++counter);
        grove_lcd_rgb_set_cursor(lcd_handle, 0, 1);
        grove_lcd_rgb_printf(lcd_handle, "Color: %s", color_names[color_index]);
        
        ESP_LOGI(TAG, "Counter: %d, Color: %s", counter, color_names[color_index]);
    }
    
    // Clean up (this code won't be reached in this example)
    grove_lcd_rgb_deinit(lcd_handle);
}