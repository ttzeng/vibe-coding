#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/ledc.h"
#include "esp_err.h"
#include "esp_log.h"
#include "sdkconfig.h"

// LED configuration
#define LED_GPIO_PIN        CONFIG_GPIO_LED   // GPIO pin for LED (change as needed)
#define LEDC_TIMER          LEDC_TIMER_0
#define LEDC_MODE           LEDC_LOW_SPEED_MODE
#define LEDC_CHANNEL        LEDC_CHANNEL_0
#define LEDC_DUTY_RES       LEDC_TIMER_13_BIT // Set duty resolution to 13 bits
#define LEDC_FREQUENCY      (4000) // Frequency in Hertz. Set frequency at 4 kHz

// Fade parameters
#define FADE_TIME_MS        (3000) // 3 seconds fade time
#define FADE_STEP_DELAY_MS  (10)   // Delay between fade steps

static const char *TAG = "LED_FADE";

/**
 * @brief Configure LEDC timer
 */
static esp_err_t ledc_timer_config_setup(void)
{
    // Prepare and then apply the LEDC PWM timer configuration
    ledc_timer_config_t ledc_timer = {
        .speed_mode       = LEDC_MODE,
        .timer_num        = LEDC_TIMER,
        .duty_resolution  = LEDC_DUTY_RES,
        .freq_hz          = LEDC_FREQUENCY,
        .clk_cfg          = LEDC_AUTO_CLK
    };
    
    esp_err_t err = ledc_timer_config(&ledc_timer);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Timer configuration failed: %s", esp_err_to_name(err));
    }
    return err;
}

/**
 * @brief Configure LEDC channel
 */
static esp_err_t ledc_channel_config_setup(void)
{
    // Prepare and then apply the LEDC PWM channel configuration
    ledc_channel_config_t ledc_channel = {
        .speed_mode     = LEDC_MODE,
        .channel        = LEDC_CHANNEL,
        .timer_sel      = LEDC_TIMER,
        .intr_type      = LEDC_INTR_DISABLE,
        .gpio_num       = LED_GPIO_PIN,
        .duty           = 0, // Set duty to 0%
        .hpoint         = 0
    };
    
    esp_err_t err = ledc_channel_config(&ledc_channel);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Channel configuration failed: %s", esp_err_to_name(err));
    }
    return err;
}

/**
 * @brief Fade LED in (0% to 100% brightness)
 */
static void fade_in(void)
{
    uint32_t max_duty = (1 << LEDC_DUTY_RES) - 1; // Maximum duty cycle
    uint32_t steps = FADE_TIME_MS / FADE_STEP_DELAY_MS;
    uint32_t duty_step = max_duty / steps;
    
    ESP_LOGI(TAG, "Starting fade in...");
    
    for (uint32_t duty = 0; duty <= max_duty; duty += duty_step) {
        // Set duty cycle
        ESP_ERROR_CHECK(ledc_set_duty(LEDC_MODE, LEDC_CHANNEL, duty));
        // Update duty to apply the new value
        ESP_ERROR_CHECK(ledc_update_duty(LEDC_MODE, LEDC_CHANNEL));
        
        vTaskDelay(pdMS_TO_TICKS(FADE_STEP_DELAY_MS));
    }
    
    // Ensure we reach maximum brightness
    ESP_ERROR_CHECK(ledc_set_duty(LEDC_MODE, LEDC_CHANNEL, max_duty));
    ESP_ERROR_CHECK(ledc_update_duty(LEDC_MODE, LEDC_CHANNEL));
    
    ESP_LOGI(TAG, "Fade in complete");
}

/**
 * @brief Fade LED out (100% to 0% brightness)
 */
static void fade_out(void)
{
    uint32_t max_duty = (1 << LEDC_DUTY_RES) - 1; // Maximum duty cycle
    uint32_t steps = FADE_TIME_MS / FADE_STEP_DELAY_MS;
    uint32_t duty_step = max_duty / steps;
    
    ESP_LOGI(TAG, "Starting fade out...");
    
    for (uint32_t duty = max_duty; duty > duty_step; duty -= duty_step) {
        // Set duty cycle
        ESP_ERROR_CHECK(ledc_set_duty(LEDC_MODE, LEDC_CHANNEL, duty));
        // Update duty to apply the new value
        ESP_ERROR_CHECK(ledc_update_duty(LEDC_MODE, LEDC_CHANNEL));
        
        vTaskDelay(pdMS_TO_TICKS(FADE_STEP_DELAY_MS));
    }
    
    // Ensure we reach minimum brightness (off)
    ESP_ERROR_CHECK(ledc_set_duty(LEDC_MODE, LEDC_CHANNEL, 0));
    ESP_ERROR_CHECK(ledc_update_duty(LEDC_MODE, LEDC_CHANNEL));
    
    ESP_LOGI(TAG, "Fade out complete");
}

/**
 * @brief LED fade task
 */
static void led_fade_task(void *pvParameters)
{
    while (1) {
        fade_in();
        vTaskDelay(pdMS_TO_TICKS(500)); // Hold at full brightness for 500ms
        
        fade_out();
        vTaskDelay(pdMS_TO_TICKS(500)); // Hold at off for 500ms
    }
}

/**
 * @brief Main application entry point
 */
void app_main(void)
{
    ESP_LOGI(TAG, "ESP32-C3 LED Fade Example Starting...");
    
    // Configure LEDC timer
    ESP_ERROR_CHECK(ledc_timer_config_setup());
    
    // Configure LEDC channel
    ESP_ERROR_CHECK(ledc_channel_config_setup());
    
    // Install LEDC fade function
    ESP_ERROR_CHECK(ledc_fade_func_install(0));
    
    ESP_LOGI(TAG, "LEDC configuration complete. Starting fade task...");
    
    // Create LED fade task
    xTaskCreate(led_fade_task, "led_fade_task", 2048, NULL, 5, NULL);
    
    ESP_LOGI(TAG, "LED fade task created successfully");
}

/* Alternative implementation using hardware fade function */
#ifdef USE_HARDWARE_FADE
/**
 * @brief Hardware-accelerated fade implementation (alternative approach)
 */
static void hardware_fade_example(void)
{
    uint32_t max_duty = (1 << LEDC_DUTY_RES) - 1;
    
    while (1) {
        ESP_LOGI(TAG, "Starting hardware fade in...");
        ESP_ERROR_CHECK(ledc_set_fade_with_time(LEDC_MODE, LEDC_CHANNEL, max_duty, FADE_TIME_MS));
        ESP_ERROR_CHECK(ledc_fade_start(LEDC_MODE, LEDC_CHANNEL, LEDC_FADE_NO_WAIT));
        vTaskDelay(pdMS_TO_TICKS(FADE_TIME_MS + 500));
        
        ESP_LOGI(TAG, "Starting hardware fade out...");
        ESP_ERROR_CHECK(ledc_set_fade_with_time(LEDC_MODE, LEDC_CHANNEL, 0, FADE_TIME_MS));
        ESP_ERROR_CHECK(ledc_fade_start(LEDC_MODE, LEDC_CHANNEL, LEDC_FADE_NO_WAIT));
        vTaskDelay(pdMS_TO_TICKS(FADE_TIME_MS + 500));
    }
}
#endif