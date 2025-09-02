#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "driver/i2c_master.h"
#include "ssd1306.h"

static const char *TAG = "MAIN";

#define I2C_MASTER_SCL_IO           9    // GPIO9 for SCL
#define I2C_MASTER_SDA_IO           8    // GPIO8 for SDA
#define I2C_MASTER_FREQ_HZ          400000

static i2c_master_bus_handle_t i2c_bus_handle;

static esp_err_t i2c_master_init(void)
{
    i2c_master_bus_config_t i2c_mst_config = {
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .i2c_port = I2C_NUM_0,
        .scl_io_num = I2C_MASTER_SCL_IO,
        .sda_io_num = I2C_MASTER_SDA_IO,
        .glitch_ignore_cnt = 7,
        .flags.enable_internal_pullup = true,
    };
    
    esp_err_t err = i2c_new_master_bus(&i2c_mst_config, &i2c_bus_handle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to create I2C master bus: %s", esp_err_to_name(err));
        return err;
    }
    
    ESP_LOGI(TAG, "I2C master bus created successfully");
    return ESP_OK;
}

void app_main(void)
{
    ESP_LOGI(TAG, "Starting SSD1306 OLED Display Demo");
    
    // Initialize I2C Master
    ESP_ERROR_CHECK(i2c_master_init());
    ESP_LOGI(TAG, "I2C master initialized successfully");
    
    // Initialize SSD1306
    ssd1306_handle_t display = ssd1306_create(i2c_bus_handle, SSD1306_I2C_ADDRESS);
    if (display == NULL) {
        ESP_LOGE(TAG, "Failed to create SSD1306 handle");
        return;
    }
    
    ESP_ERROR_CHECK(ssd1306_init(display));
    ESP_LOGI(TAG, "SSD1306 initialized successfully");
    
    // Clear display
    ssd1306_clear_screen(display, 0x00);
    ssd1306_refresh_gram(display);
    
    // Demo loop
    int counter = 0;
    char text_buffer[32];
    
    while (1) {
        // Clear screen
        ssd1306_clear_screen(display, 0x00);
        
        // Display title
        ssd1306_show_string(display, 0, 0, "ESP32-C3 Demo", 16, 1);
        
        // Display counter
        snprintf(text_buffer, sizeof(text_buffer), "Count: %d", counter);
        ssd1306_show_string(display, 0, 16, text_buffer, 16, 1);
        
        // Display some patterns
        ssd1306_draw_point(display, 64, 32, SSD1306_COLOR_WHITE);
        ssd1306_draw_line(display, 0, 32, 127, 32, SSD1306_COLOR_WHITE);
        ssd1306_draw_rectangle(display, 10, 40, 50, 20, SSD1306_COLOR_WHITE);
        
        // Refresh display
        ssd1306_refresh_gram(display);
        
        counter++;
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
