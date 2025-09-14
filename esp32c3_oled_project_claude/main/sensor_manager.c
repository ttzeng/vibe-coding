#include <math.h>
#include "sensor_manager.h"
#include "esp_log.h"
#include "esp_random.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static const char *TAG = "SENSOR_MGR";

static sensor_data_t g_sensor_data = {0};
static bool initialized = false;

esp_err_t sensor_manager_init(void)
{
    // Initialize sensor hardware here
    // For this demo, we'll simulate sensors
    
    g_sensor_data.temperature = 22.5;
    g_sensor_data.humidity = 45.0;
    g_sensor_data.pressure = 1013.25;
    g_sensor_data.light_level = 500;
    g_sensor_data.data_valid = false;
    g_sensor_data.last_update = 0;
    
    initialized = true;
    ESP_LOGI(TAG, "Sensor manager initialized (simulated sensors)");
    return ESP_OK;
}

esp_err_t sensor_manager_update(void)
{
    if (!initialized) {
        return ESP_ERR_INVALID_STATE;
    }
    
    uint32_t now = xTaskGetTickCount() * portTICK_PERIOD_MS;
    
    // Simulate sensor readings with realistic variations
    esp_err_t ret = ESP_OK;
    
    // Temperature: 20-25°C with slow variations
    static float temp_base = 22.5;
    temp_base += (esp_random() % 100 - 50) * 0.01; // ±0.5°C variation
    if (temp_base < 20.0) temp_base = 20.0;
    if (temp_base > 25.0) temp_base = 25.0;
    
    ret |= sensor_read_temperature(&g_sensor_data.temperature);
    
    // Humidity: 40-60% with medium variations
    static float hum_base = 45.0;
    hum_base += (esp_random() % 100 - 50) * 0.05; // ±2.5% variation
    if (hum_base < 40.0) hum_base = 40.0;
    if (hum_base > 60.0) hum_base = 60.0;
    
    ret |= sensor_read_humidity(&g_sensor_data.humidity);
    
    // Pressure: 1010-1020 hPa with slow variations
    static float pressure_base = 1013.25;
    pressure_base += (esp_random() % 100 - 50) * 0.02; // ±1 hPa variation
    if (pressure_base < 1010.0) pressure_base = 1010.0;
    if (pressure_base > 1020.0) pressure_base = 1020.0;
    
    ret |= sensor_read_pressure(&g_sensor_data.pressure);
    
    // Light level: 0-1000 with day/night cycle simulation
    uint16_t light = 500 + 400 * sin(now * 0.0001); // Simulate day/night cycle
    light += esp_random() % 100 - 50; // Add some noise
    if (light > 1000) light = 1000;
    
    ret |= sensor_read_light(&g_sensor_data.light_level);
    
    g_sensor_data.data_valid = (ret == ESP_OK);
    g_sensor_data.last_update = now;
    
    if (ret == ESP_OK) {
        ESP_LOGD(TAG, "Sensors updated: T=%.1f°C, H=%.1f%%, P=%.1fhPa, L=%d",
                g_sensor_data.temperature, g_sensor_data.humidity,
                g_sensor_data.pressure, g_sensor_data.light_level);
    }
    
    return ret;
}

sensor_data_t* sensor_manager_get_data(void)
{
    return &g_sensor_data;
}

bool sensor_manager_is_data_valid(void)
{
    uint32_t now = xTaskGetTickCount() * portTICK_PERIOD_MS;
    return g_sensor_data.data_valid && (now - g_sensor_data.last_update < 5000);
}

// Individual sensor reading functions (simulated)
esp_err_t sensor_read_temperature(float *temperature)
{
    if (temperature == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    
    // Simulate I2C/SPI communication delay
    vTaskDelay(pdMS_TO_TICKS(10));
    
    // Simulate realistic temperature with noise
    static float base_temp = 22.5;
    base_temp += (esp_random() % 20 - 10) * 0.01; // ±0.1°C noise
    
    // Add some seasonal/daily variation
    uint32_t time_factor = xTaskGetTickCount() * portTICK_PERIOD_MS;
    float daily_variation = 2.0 * sin(time_factor * 0.0001);
    
    *temperature = base_temp + daily_variation;
    
    return ESP_OK;
}

esp_err_t sensor_read_humidity(float *humidity)
{
    if (humidity == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    
    vTaskDelay(pdMS_TO_TICKS(10));
    
    static float base_humidity = 45.0;
    base_humidity += (esp_random() % 40 - 20) * 0.05; // ±1% noise
    
    // Humidity inversely related to temperature (roughly)
    uint32_t time_factor = xTaskGetTickCount() * portTICK_PERIOD_MS;
    float temp_influence = -1.0 * sin(time_factor * 0.0001);
    
    *humidity = base_humidity + temp_influence;
    
    // Clamp to realistic range
    if (*humidity < 0) *humidity = 0;
    if (*humidity > 100) *humidity = 100;
    
    return ESP_OK;
}

esp_err_t sensor_read_pressure(float *pressure)
{
    if (pressure == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    
    vTaskDelay(pdMS_TO_TICKS(15));
    
    static float base_pressure = 1013.25;
    base_pressure += (esp_random() % 10 - 5) * 0.1; // ±0.5 hPa noise
    
    // Slow pressure changes
    uint32_t time_factor = xTaskGetTickCount() * portTICK_PERIOD_MS;
    float weather_pattern = 5.0 * sin(time_factor * 0.00005);
    
    *pressure = base_pressure + weather_pattern;
    
    return ESP_OK;
}

esp_err_t sensor_read_light(uint16_t *light_level)
{
    if (light_level == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    
    vTaskDelay(pdMS_TO_TICKS(5));
    
    // Simulate day/night cycle
    uint32_t time_factor = xTaskGetTickCount() * portTICK_PERIOD_MS;
    float day_cycle = sin(time_factor * 0.0002); // Slow day/night cycle
    
    uint16_t base_light = 500 + 400 * day_cycle;
    base_light += esp_random() % 100 - 50; // Add noise
    
    if (base_light > 1000) base_light = 1000;
    if (base_light < 0) base_light = 0;
    
    *light_level = base_light;
    
    return ESP_OK;
}
