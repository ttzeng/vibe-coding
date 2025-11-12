#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "xpt2046.h"

static const char *TAG = "MAIN";

// Define your GPIO pins here (adjust according to your hardware)
#define PIN_NUM_MISO    5
#define PIN_NUM_MOSI    6
#define PIN_NUM_CLK     4
#define PIN_NUM_CS      7
#define PIN_NUM_IRQ     3   // Set to GPIO_NUM_NC if not connected

// Touch screen configuration
xpt2046_t touch_dev;

void touch_task(void *pvParameter)
{
    xpt2046_touch_t touch_data;
    bool was_touched = false;

    ESP_LOGI(TAG, "Touch task started");

    while (1) {
        // Read touch data
        esp_err_t ret = xpt2046_read_touch(&touch_dev, &touch_data);
        
        if (ret == ESP_OK) {
            if (touch_data.touched) {
                // Touch detected
                if (!was_touched) {
                    ESP_LOGI(TAG, "=== Touch Pressed ===");
                    was_touched = true;
                }
                
                ESP_LOGI(TAG, "Position: X=%d, Y=%d (Raw: %d, %d) Pressure: %d",
                         touch_data.x_calibrated,
                         touch_data.y_calibrated,
                         touch_data.x_raw,
                         touch_data.y_raw,
                         touch_data.z_raw);
                
                // Add your touch handling code here
                // For example: draw on screen, detect button press, etc.
                
            } else if (was_touched) {
                ESP_LOGI(TAG, "=== Touch Released ===");
                was_touched = false;
            }
        } else {
            ESP_LOGE(TAG, "Failed to read touch: %s", esp_err_to_name(ret));
        }

        // Poll rate (adjust as needed)
        vTaskDelay(pdMS_TO_TICKS(50));  // 20 Hz polling
    }
}

void app_main(void)
{
    ESP_LOGI(TAG, "Starting XPT2046 Touch Screen Example");
    ESP_LOGI(TAG, "ESP-IDF Version: %s", esp_get_idf_version());

    // Initialize XPT2046
    esp_err_t ret = xpt2046_init(&touch_dev, 
                                 SPI2_HOST,
                                 PIN_NUM_MISO,
                                 PIN_NUM_MOSI,
                                 PIN_NUM_CLK,
                                 PIN_NUM_CS,
                                 PIN_NUM_IRQ);

    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to initialize XPT2046: %s", esp_err_to_name(ret));
        return;
    }

    // Optional: Set calibration values
    // You need to determine these values for your specific screen
    // by reading corner positions and mapping them
    touch_dev.screen_width = 320;
    touch_dev.screen_height = 240;
    xpt2046_calibrate(&touch_dev, 200, 3900, 200, 3900);

    // Create touch reading task
    xTaskCreate(touch_task, "touch_task", 4096, NULL, 5, NULL);

    ESP_LOGI(TAG, "Application started. Touch the screen to see coordinates.");
}
