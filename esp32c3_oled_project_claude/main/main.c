#include <stdio.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_log.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_sntp.h"
#include "nvs_flash.h"
#include "driver/gpio.h"
#include "driver/i2c_master.h"

#include "app_config.h"
#include "ssd1306.h"
#include "display_manager.h"
#include "menu_system.h"
#include "sensor_manager.h"
#include "wifi_manager.h"
#include "animations.h"

static const char *TAG = "MAIN";

// Global handles
static i2c_master_bus_handle_t i2c_bus_handle;
static ssd1306_handle_t display_handle;
static display_manager_handle_t display_manager;

// Application state
static display_mode_t current_mode = DISPLAY_MODE_CLOCK;
static bool button_pressed = false;
static uint32_t last_button_press = 0;

// Button interrupt handler
static void IRAM_ATTR button_isr_handler(void* arg)
{
    uint32_t now = xTaskGetTickCount() * portTICK_PERIOD_MS;
    if (now - last_button_press > 200) { // Debounce
        button_pressed = true;
        last_button_press = now;
    }
}

static esp_err_t init_hardware(void)
{
    esp_err_t ret;
    
    // Initialize I2C Master
    i2c_master_bus_config_t i2c_config = {
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .i2c_port = I2C_NUM_0,
        .scl_io_num = I2C_MASTER_SCL_IO,
        .sda_io_num = I2C_MASTER_SDA_IO,
        .glitch_ignore_cnt = 7,
        .flags.enable_internal_pullup = true,
    };
    
    ret = i2c_new_master_bus(&i2c_config, &i2c_bus_handle);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to create I2C master bus: %s", esp_err_to_name(ret));
        return ret;
    }
    
    // Initialize SSD1306
    display_handle = ssd1306_create(i2c_bus_handle, SSD1306_I2C_ADDRESS);
    if (display_handle == NULL) {
        ESP_LOGE(TAG, "Failed to create SSD1306 handle");
        return ESP_FAIL;
    }
    
    ret = ssd1306_init(display_handle);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to initialize SSD1306: %s", esp_err_to_name(ret));
        return ret;
    }
    
    // Initialize button
    gpio_config_t button_config = {
        .pin_bit_mask = (1ULL << BUTTON_GPIO),
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_ENABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_NEGEDGE,
    };
    gpio_config(&button_config);
    gpio_install_isr_service(0);
    gpio_isr_handler_add(BUTTON_GPIO, button_isr_handler, NULL);
    
    ESP_LOGI(TAG, "Hardware initialization completed");
    return ESP_OK;
}

static void init_time_sync(void)
{
    ESP_LOGI(TAG, "Initializing SNTP");
    esp_sntp_setoperatingmode(SNTP_OPMODE_POLL);
    esp_sntp_setservername(0, "pool.ntp.org");
    esp_sntp_init();
    
    // Set timezone to your local timezone
    setenv("TZ", "EST5EDT,M3.2.0/2,M11.1.0", 1);
    tzset();
}

static void handle_button_press(void)
{
    if (button_pressed) {
        button_pressed = false;
        
        // Cycle through display modes
        current_mode = (current_mode + 1) % DISPLAY_MODE_MAX;
        display_manager_set_mode(display_manager, current_mode);
        
        ESP_LOGI(TAG, "Display mode changed to: %d", current_mode);
    }
}

static void display_task(void *pvParameters)
{
    TickType_t last_wake_time = xTaskGetTickCount();
    
    while (1) {
        handle_button_press();
        display_manager_update(display_manager);
        
        vTaskDelayUntil(&last_wake_time, pdMS_TO_TICKS(DISPLAY_UPDATE_INTERVAL_MS));
    }
}

static void sensor_task(void *pvParameters)
{
    TickType_t last_wake_time = xTaskGetTickCount();
    
    while (1) {
        sensor_manager_update();
        vTaskDelayUntil(&last_wake_time, pdMS_TO_TICKS(SENSOR_READ_INTERVAL_MS));
    }
}

void app_main(void)
{
    ESP_LOGI(TAG, "Starting %s v%s", APP_NAME, APP_VERSION);
    
    // Initialize NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);
    
    // Initialize hardware
    ESP_ERROR_CHECK(init_hardware());
    
    // Initialize managers
    display_manager = display_manager_create(display_handle);
    if (display_manager == NULL) {
        ESP_LOGE(TAG, "Failed to create display manager");
        return;
    }
    
    sensor_manager_init();
    menu_system_init();
    animations_init();
    
    // Show startup screen
    display_manager_show_startup(display_manager);
    vTaskDelay(pdMS_TO_TICKS(2000));
    
    // Initialize WiFi (non-blocking)
    wifi_manager_init();
    
    // Initialize time sync after WiFi
    init_time_sync();
    
    // Start tasks
    xTaskCreate(display_task, "display_task", 4096, NULL, 5, NULL);
    xTaskCreate(sensor_task, "sensor_task", 4096, NULL, 3, NULL);
    
    ESP_LOGI(TAG, "Application started successfully");
    
    // Main loop
    while (1) {
        vTaskDelay(pdMS_TO_TICKS(1000));
        
        // Periodic maintenance tasks
        static int counter = 0;
        if (++counter % 60 == 0) { // Every minute
            ESP_LOGI(TAG, "System uptime: %d minutes", counter);
            ESP_LOGI(TAG, "Free heap: %d bytes", esp_get_free_heap_size());
        }
    }
}
