#include <stdio.h>
#include <math.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "adxl345.h"

static const char *TAG = "ADXL345_DEMO";

// GPIO pins for I2C (adjust for your ESP32-C3 board)
#define I2C_SDA_PIN GPIO_NUM_8
#define I2C_SCL_PIN GPIO_NUM_9

void app_main(void)
{
    ESP_LOGI(TAG, "ADXL345 ESP32-C3 Demo Starting...");

    // Configure ADXL345
    adxl345_config_t adxl345_config = {
        .sda_pin = I2C_SDA_PIN,
        .scl_pin = I2C_SCL_PIN,
        .clk_speed = 400000,  // 400kHz
        .enable_pullup = true
    };

    // Initialize the ADXL345
    adxl345_handle_t *adxl345_handle = NULL;
    esp_err_t ret = adxl345_init(&adxl345_config, &adxl345_handle);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to initialize ADXL345: %s", esp_err_to_name(ret));
        return;
    }

    // Set accelerometer range to ±4g
    ret = adxl345_set_range(adxl345_handle, ADXL345_RANGE_4G);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to set range: %s", esp_err_to_name(ret));
        adxl345_deinit(adxl345_handle);
        return;
    }

    // Set data rate to 100Hz
    ret = adxl345_set_data_rate(adxl345_handle, ADXL345_BW_RATE_100HZ);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to set data rate: %s", esp_err_to_name(ret));
        adxl345_deinit(adxl345_handle);
        return;
    }

    ESP_LOGI(TAG, "ADXL345 configuration complete. Starting data acquisition...");

    // Main loop - read accelerometer data
    adxl345_accel_data_t accel_data;
    int16_t raw_x, raw_y, raw_z;
    uint32_t sample_count = 0;
    
    while (1) {
        sample_count++;
        
        // Read acceleration data in g units
        ret = adxl345_read_acceleration(adxl345_handle, &accel_data);
        if (ret == ESP_OK) {
            ESP_LOGI(TAG, "[%lu] Acceleration (g): X=%.3f, Y=%.3f, Z=%.3f", 
                     sample_count, accel_data.x, accel_data.y, accel_data.z);
        } else {
            ESP_LOGE(TAG, "Failed to read acceleration: %s", esp_err_to_name(ret));
        }

        // Also demonstrate raw data reading
        ret = adxl345_read_raw_data(adxl345_handle, &raw_x, &raw_y, &raw_z);
        if (ret == ESP_OK) {
            ESP_LOGI(TAG, "[%lu] Raw data: X=%d, Y=%d, Z=%d", sample_count, raw_x, raw_y, raw_z);
        }

        // Calculate magnitude for activity detection
        if (ret == ESP_OK) {
            float magnitude = sqrtf(accel_data.x * accel_data.x + 
                                   accel_data.y * accel_data.y + 
                                   accel_data.z * accel_data.z);
            ESP_LOGI(TAG, "[%lu] Acceleration magnitude: %.3f g", sample_count, magnitude);
            
            // Simple activity detection
            if (magnitude > 1.2f) {
                ESP_LOGI(TAG, "*** MOTION DETECTED ***");
            }

            // Tilt detection
            float tilt_x = asinf(accel_data.x) * 180.0f / M_PI;
            float tilt_y = asinf(accel_data.y) * 180.0f / M_PI;
            ESP_LOGI(TAG, "[%lu] Tilt angles: X=%.1f°, Y=%.1f°", sample_count, tilt_x, tilt_y);
        }

        ESP_LOGI(TAG, "----------------------------------------");
        vTaskDelay(pdMS_TO_TICKS(500)); // Read every 500ms
    }

    // Cleanup (this won't be reached in this example, but good practice)
    adxl345_deinit(adxl345_handle);
}
