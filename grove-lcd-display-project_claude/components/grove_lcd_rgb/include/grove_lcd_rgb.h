/**
 * @file grove_lcd_rgb.h
 * @brief Grove LCD RGB Backlight Driver for ESP-IDF
 * 
 * This library provides an easy-to-use interface for the Grove LCD RGB Backlight
 * module using the latest ESP-IDF I2C master API.
 * 
 * @author ESP32 Developer
 * @date 2025
 */

#pragma once

#include "esp_err.h"
#include "driver/i2c_master.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Grove LCD RGB handle type
 */
typedef struct grove_lcd_rgb_t* grove_lcd_rgb_handle_t;

/**
 * @brief Grove LCD RGB configuration structure
 */
typedef struct {
    int scl_io_num;                 /*!< GPIO number for SCL */
    int sda_io_num;                 /*!< GPIO number for SDA */
    uint32_t clk_speed_hz;          /*!< I2C clock frequency */
    i2c_port_t i2c_port;           /*!< I2C port number */
} grove_lcd_rgb_config_t;

/**
 * @brief RGB color structure
 */
typedef struct {
    uint8_t red;    /*!< Red component (0-255) */
    uint8_t green;  /*!< Green component (0-255) */
    uint8_t blue;   /*!< Blue component (0-255) */
} grove_lcd_rgb_color_t;

/**
 * @brief Default configuration for Grove LCD RGB
 */
#define GROVE_LCD_RGB_CONFIG_DEFAULT() {          \
    .scl_io_num = CONFIG_GROVE_LCD_SCL_GPIO,      \
    .sda_io_num = CONFIG_GROVE_LCD_SDA_GPIO,      \
    .clk_speed_hz = 100000,                       \
    .i2c_port = I2C_NUM_0,                        \
}

/**
 * @brief Predefined colors
 */
#define GROVE_LCD_COLOR_RED     {255, 0, 0}
#define GROVE_LCD_COLOR_GREEN   {0, 255, 0}
#define GROVE_LCD_COLOR_BLUE    {0, 0, 255}
#define GROVE_LCD_COLOR_YELLOW  {255, 255, 0}
#define GROVE_LCD_COLOR_MAGENTA {255, 0, 255}
#define GROVE_LCD_COLOR_CYAN    {0, 255, 255}
#define GROVE_LCD_COLOR_WHITE   {255, 255, 255}
#define GROVE_LCD_COLOR_BLACK   {0, 0, 0}

/**
 * @brief Initialize Grove LCD RGB module
 * 
 * @param config Pointer to configuration structure
 * @param handle Pointer to handle that will be set
 * @return esp_err_t ESP_OK on success
 */
esp_err_t grove_lcd_rgb_init(const grove_lcd_rgb_config_t *config, grove_lcd_rgb_handle_t *handle);

/**
 * @brief Wait for LCD to be fully ready and perform final setup
 * Call this after grove_lcd_rgb_init() and before first use
 * 
 * @param handle Handle to the Grove LCD RGB device
 * @return esp_err_t ESP_OK on success
 */
esp_err_t grove_lcd_rgb_ready(grove_lcd_rgb_handle_t handle);

/**
 * @brief Deinitialize Grove LCD RGB module
 * 
 * @param handle Handle to the Grove LCD RGB device
 * @return esp_err_t ESP_OK on success
 */
esp_err_t grove_lcd_rgb_deinit(grove_lcd_rgb_handle_t handle);

/**
 * @brief Clear the LCD display
 * 
 * @param handle Handle to the Grove LCD RGB device
 * @return esp_err_t ESP_OK on success
 */
esp_err_t grove_lcd_rgb_clear(grove_lcd_rgb_handle_t handle);

/**
 * @brief Set cursor position
 * 
 * @param handle Handle to the Grove LCD RGB device
 * @param col Column position (0-15)
 * @param row Row position (0-1)
 * @return esp_err_t ESP_OK on success
 */
esp_err_t grove_lcd_rgb_set_cursor(grove_lcd_rgb_handle_t handle, uint8_t col, uint8_t row);

/**
 * @brief Print string to LCD
 * 
 * @param handle Handle to the Grove LCD RGB device
 * @param str String to print
 * @return esp_err_t ESP_OK on success
 */
esp_err_t grove_lcd_rgb_print(grove_lcd_rgb_handle_t handle, const char *str);

/**
 * @brief Print formatted string to LCD
 * 
 * @param handle Handle to the Grove LCD RGB device
 * @param format Format string (printf style)
 * @param ... Variable arguments
 * @return esp_err_t ESP_OK on success
 */
esp_err_t grove_lcd_rgb_printf(grove_lcd_rgb_handle_t handle, const char *format, ...);

/**
 * @brief Set RGB backlight color
 * 
 * @param handle Handle to the Grove LCD RGB device
 * @param color RGB color structure
 * @return esp_err_t ESP_OK on success
 */
esp_err_t grove_lcd_rgb_set_color(grove_lcd_rgb_handle_t handle, grove_lcd_rgb_color_t color);

/**
 * @brief Set RGB backlight color with individual values
 * 
 * @param handle Handle to the Grove LCD RGB device
 * @param red Red component (0-255)
 * @param green Green component (0-255)
 * @param blue Blue component (0-255)
 * @return esp_err_t ESP_OK on success
 */
esp_err_t grove_lcd_rgb_set_color_rgb(grove_lcd_rgb_handle_t handle, uint8_t red, uint8_t green, uint8_t blue);

/**
 * @brief Turn display on/off
 * 
 * @param handle Handle to the Grove LCD RGB device
 * @param on true to turn on, false to turn off
 * @return esp_err_t ESP_OK on success
 */
esp_err_t grove_lcd_rgb_display(grove_lcd_rgb_handle_t handle, bool on);

/**
 * @brief Turn cursor on/off
 * 
 * @param handle Handle to the Grove LCD RGB device
 * @param on true to turn on, false to turn off
 * @return esp_err_t ESP_OK on success
 */
esp_err_t grove_lcd_rgb_cursor(grove_lcd_rgb_handle_t handle, bool on);

/**
 * @brief Turn cursor blink on/off
 * 
 * @param handle Handle to the Grove LCD RGB device
 * @param on true to turn on, false to turn off
 * @return esp_err_t ESP_OK on success
 */
esp_err_t grove_lcd_rgb_blink(grove_lcd_rgb_handle_t handle, bool on);

/**
 * @brief Home cursor (move to position 0,0)
 * 
 * @param handle Handle to the Grove LCD RGB device
 * @return esp_err_t ESP_OK on success
 */
esp_err_t grove_lcd_rgb_home(grove_lcd_rgb_handle_t handle);

#ifdef __cplusplus
}
#endif