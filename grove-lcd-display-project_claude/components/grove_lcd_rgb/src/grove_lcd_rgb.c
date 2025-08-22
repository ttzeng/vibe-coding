/**
 * @file grove_lcd_rgb.c
 * @brief Grove LCD RGB Backlight Driver Implementation
 */

#include "grove_lcd_rgb.h"
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_check.h"

static const char *TAG = "grove_lcd_rgb";

// I2C addresses
#define LCD_ADDRESS     0x3E    // LCD display address
#define RGB_ADDRESS     0x62    // RGB backlight address

// LCD commands
#define LCD_CLEARDISPLAY        0x01
#define LCD_RETURNHOME          0x02
#define LCD_ENTRYMODESET        0x04
#define LCD_DISPLAYCONTROL      0x08
#define LCD_CURSORSHIFT         0x10
#define LCD_FUNCTIONSET         0x20
#define LCD_SETCGRAMADDR        0x40
#define LCD_SETDDRAMADDR        0x80

// LCD flags
#define LCD_ENTRYRIGHT          0x00
#define LCD_ENTRYLEFT           0x02
#define LCD_ENTRYSHIFTINCREMENT 0x01
#define LCD_ENTRYSHIFTDECREMENT 0x00

#define LCD_DISPLAYON           0x04
#define LCD_DISPLAYOFF          0x00
#define LCD_CURSORON            0x02
#define LCD_CURSOROFF           0x00
#define LCD_BLINKON             0x01
#define LCD_BLINKOFF            0x00

#define LCD_DISPLAYMOVE         0x08
#define LCD_CURSORMOVE          0x00
#define LCD_MOVERIGHT           0x04
#define LCD_MOVELEFT            0x00

#define LCD_8BITMODE            0x10
#define LCD_4BITMODE            0x00
#define LCD_2LINE               0x08
#define LCD_1LINE               0x00
#define LCD_5x10DOTS            0x04
#define LCD_5x8DOTS             0x00

// RGB backlight registers
#define REG_RED         0x04
#define REG_GREEN       0x03
#define REG_BLUE        0x02
#define REG_MODE1       0x00
#define REG_MODE2       0x01
#define REG_OUTPUT      0x08

// Timeout for I2C operations
#define I2C_TIMEOUT_MS  1000

/**
 * @brief Grove LCD RGB device structure
 */
struct grove_lcd_rgb_t {
    i2c_master_bus_handle_t i2c_bus_handle;
    i2c_master_dev_handle_t lcd_dev_handle;
    i2c_master_dev_handle_t rgb_dev_handle;
    uint8_t display_control;
    uint8_t entry_mode;
    uint8_t rows;
    uint8_t cols;
};

/**
 * @brief Write command to LCD
 */
static esp_err_t lcd_write_cmd(grove_lcd_rgb_handle_t handle, uint8_t cmd)
{
    uint8_t data[2] = {0x80, cmd};  // 0x80 = command mode
    return i2c_master_transmit(handle->lcd_dev_handle, data, sizeof(data), I2C_TIMEOUT_MS);
}

/**
 * @brief Write data to LCD
 */
static esp_err_t lcd_write_data(grove_lcd_rgb_handle_t handle, uint8_t data)
{
    uint8_t buffer[2] = {0x40, data};  // 0x40 = data mode
    return i2c_master_transmit(handle->lcd_dev_handle, buffer, sizeof(buffer), I2C_TIMEOUT_MS);
}

/**
 * @brief Write to RGB controller register
 */
static esp_err_t rgb_write_reg(grove_lcd_rgb_handle_t handle, uint8_t reg, uint8_t data)
{
    uint8_t buffer[2] = {reg, data};
    return i2c_master_transmit(handle->rgb_dev_handle, buffer, sizeof(buffer), I2C_TIMEOUT_MS);
}

/**
 * @brief Initialize RGB backlight controller
 */
static esp_err_t rgb_init(grove_lcd_rgb_handle_t handle)
{
    ESP_RETURN_ON_ERROR(rgb_write_reg(handle, REG_MODE1, 0x00), TAG, "Failed to set MODE1");
    ESP_RETURN_ON_ERROR(rgb_write_reg(handle, REG_MODE2, 0x01), TAG, "Failed to set MODE2");
    ESP_RETURN_ON_ERROR(rgb_write_reg(handle, REG_OUTPUT, 0xFF), TAG, "Failed to set OUTPUT");
    
    ESP_LOGI(TAG, "RGB backlight initialized");
    return ESP_OK;
}

/**
 * @brief Initialize LCD controller
 */
static esp_err_t lcd_controller_init(grove_lcd_rgb_handle_t handle)
{
    vTaskDelay(pdMS_TO_TICKS(100));  // Extended wait for power stabilization
    
    // Reset sequence - send function set multiple times as per HD44780 datasheet
    ESP_RETURN_ON_ERROR(lcd_write_cmd(handle, LCD_FUNCTIONSET | LCD_8BITMODE | LCD_2LINE | LCD_5x8DOTS), 
                       TAG, "Failed to set function (1st)");
    vTaskDelay(pdMS_TO_TICKS(10));
    
    ESP_RETURN_ON_ERROR(lcd_write_cmd(handle, LCD_FUNCTIONSET | LCD_8BITMODE | LCD_2LINE | LCD_5x8DOTS), 
                       TAG, "Failed to set function (2nd)");
    vTaskDelay(pdMS_TO_TICKS(5));
    
    ESP_RETURN_ON_ERROR(lcd_write_cmd(handle, LCD_FUNCTIONSET | LCD_8BITMODE | LCD_2LINE | LCD_5x8DOTS), 
                       TAG, "Failed to set function (3rd)");
    vTaskDelay(pdMS_TO_TICKS(5));
    
    // Display control: display OFF first
    ESP_RETURN_ON_ERROR(lcd_write_cmd(handle, LCD_DISPLAYCONTROL | LCD_DISPLAYOFF), TAG, "Failed to turn off display");
    vTaskDelay(pdMS_TO_TICKS(5));
    
    // Clear display
    ESP_RETURN_ON_ERROR(lcd_write_cmd(handle, LCD_CLEARDISPLAY), TAG, "Failed to clear display");
    vTaskDelay(pdMS_TO_TICKS(10));  // Clear command needs longer delay
    
    // Entry mode: cursor moves right, no display shift
    handle->entry_mode = LCD_ENTRYMODESET | LCD_ENTRYLEFT | LCD_ENTRYSHIFTDECREMENT;
    ESP_RETURN_ON_ERROR(lcd_write_cmd(handle, handle->entry_mode), TAG, "Failed to set entry mode");
    vTaskDelay(pdMS_TO_TICKS(5));
    
    // Display control: display on, cursor off, blink off
    handle->display_control = LCD_DISPLAYCONTROL | LCD_DISPLAYON | LCD_CURSOROFF | LCD_BLINKOFF;
    ESP_RETURN_ON_ERROR(lcd_write_cmd(handle, handle->display_control), TAG, "Failed to set display control");
    vTaskDelay(pdMS_TO_TICKS(5));
    
    // Home cursor
    ESP_RETURN_ON_ERROR(lcd_write_cmd(handle, LCD_RETURNHOME), TAG, "Failed to home cursor");
    vTaskDelay(pdMS_TO_TICKS(10));  // Home command needs longer delay
    
    // Additional clear to ensure proper initialization
    ESP_RETURN_ON_ERROR(lcd_write_cmd(handle, LCD_CLEARDISPLAY), TAG, "Failed final clear");
    vTaskDelay(pdMS_TO_TICKS(10));

    ESP_LOGI(TAG, "LCD controller initialized");
    return ESP_OK;
}

esp_err_t grove_lcd_rgb_init(const grove_lcd_rgb_config_t *config, grove_lcd_rgb_handle_t *handle)
{
    ESP_RETURN_ON_FALSE(config && handle, ESP_ERR_INVALID_ARG, TAG, "Invalid arguments");
    
    // Allocate handle
    *handle = calloc(1, sizeof(struct grove_lcd_rgb_t));
    ESP_RETURN_ON_FALSE(*handle, ESP_ERR_NO_MEM, TAG, "No memory for handle");
    
    grove_lcd_rgb_handle_t dev = *handle;
    dev->rows = 2;
    dev->cols = 16;
    
    // Create I2C bus
    i2c_master_bus_config_t i2c_mst_config = {
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .i2c_port = config->i2c_port,
        .scl_io_num = config->scl_io_num,
        .sda_io_num = config->sda_io_num,
        .glitch_ignore_cnt = 7,
        .flags.enable_internal_pullup = true,
    };
    
    esp_err_t ret = i2c_new_master_bus(&i2c_mst_config, &dev->i2c_bus_handle);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to create I2C bus: %s", esp_err_to_name(ret));
        goto error;
    }

    // Add LCD device
    i2c_device_config_t lcd_dev_cfg = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address = LCD_ADDRESS,
        .scl_speed_hz = config->clk_speed_hz,
    };
    
    ret = i2c_master_bus_add_device(dev->i2c_bus_handle, &lcd_dev_cfg, &dev->lcd_dev_handle);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to add LCD device: %s", esp_err_to_name(ret));
        goto error;
    }

    // Add RGB device
    i2c_device_config_t rgb_dev_cfg = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address = RGB_ADDRESS,
        .scl_speed_hz = config->clk_speed_hz,
    };
    
    ret = i2c_master_bus_add_device(dev->i2c_bus_handle, &rgb_dev_cfg, &dev->rgb_dev_handle);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to add RGB device: %s", esp_err_to_name(ret));
        goto error;
    }

    // Initialize RGB controller
    ret = rgb_init(dev);
    if (ret != ESP_OK) {
        goto error;
    }
    
    // Initialize LCD controller
    ret = lcd_controller_init(dev);
    if (ret != ESP_OK) {
        goto error;
    }

    ESP_LOGI(TAG, "Grove LCD RGB initialized successfully");
    return ESP_OK;

error:
    if (dev->i2c_bus_handle) {
        i2c_del_master_bus(dev->i2c_bus_handle);
    }
    free(dev);
    *handle = NULL;
    return ret;
}

esp_err_t grove_lcd_rgb_ready(grove_lcd_rgb_handle_t handle)
{
    ESP_RETURN_ON_FALSE(handle, ESP_ERR_INVALID_ARG, TAG, "Invalid handle");
    
    // Additional stabilization delay
    vTaskDelay(pdMS_TO_TICKS(100));
    
    // Perform final initialization sequence
    ESP_RETURN_ON_ERROR(lcd_write_cmd(handle, LCD_CLEARDISPLAY), TAG, "Failed ready clear");
    vTaskDelay(pdMS_TO_TICKS(10));
    
    ESP_RETURN_ON_ERROR(lcd_write_cmd(handle, LCD_RETURNHOME), TAG, "Failed ready home");
    vTaskDelay(pdMS_TO_TICKS(10));
    
    // Set cursor to position 0,0 explicitly
    ESP_RETURN_ON_ERROR(grove_lcd_rgb_set_cursor(handle, 0, 0), TAG, "Failed to set initial cursor");
    
    ESP_LOGI(TAG, "Grove LCD RGB ready for use");
    return ESP_OK;
}

esp_err_t grove_lcd_rgb_deinit(grove_lcd_rgb_handle_t handle)
{
    ESP_RETURN_ON_FALSE(handle, ESP_ERR_INVALID_ARG, TAG, "Invalid handle");
    
    if (handle->i2c_bus_handle) {
        i2c_del_master_bus(handle->i2c_bus_handle);
    }
    
    free(handle);
    ESP_LOGI(TAG, "Grove LCD RGB deinitialized");
    return ESP_OK;
}

esp_err_t grove_lcd_rgb_clear(grove_lcd_rgb_handle_t handle)
{
    ESP_RETURN_ON_FALSE(handle, ESP_ERR_INVALID_ARG, TAG, "Invalid handle");
    
    ESP_RETURN_ON_ERROR(lcd_write_cmd(handle, LCD_CLEARDISPLAY), TAG, "Failed to clear display");
    vTaskDelay(pdMS_TO_TICKS(5));  // Clear command needs more time
    
    // Ensure cursor is at home position after clear
    ESP_RETURN_ON_ERROR(lcd_write_cmd(handle, LCD_RETURNHOME), TAG, "Failed to home cursor after clear");
    vTaskDelay(pdMS_TO_TICKS(5));  // Home command also needs time
    
    return ESP_OK;
}

esp_err_t grove_lcd_rgb_set_cursor(grove_lcd_rgb_handle_t handle, uint8_t col, uint8_t row)
{
    ESP_RETURN_ON_FALSE(handle, ESP_ERR_INVALID_ARG, TAG, "Invalid handle");
    ESP_RETURN_ON_FALSE(row < handle->rows && col < handle->cols, ESP_ERR_INVALID_ARG, TAG, "Invalid cursor position");
    
    uint8_t row_offsets[] = {0x00, 0x40, 0x14, 0x54};
    ESP_RETURN_ON_ERROR(lcd_write_cmd(handle, LCD_SETDDRAMADDR | (col + row_offsets[row])), 
                       TAG, "Failed to set cursor");
    vTaskDelay(pdMS_TO_TICKS(1));  // Small delay to ensure cursor is set
    return ESP_OK;
}

esp_err_t grove_lcd_rgb_print(grove_lcd_rgb_handle_t handle, const char *str)
{
    ESP_RETURN_ON_FALSE(handle && str, ESP_ERR_INVALID_ARG, TAG, "Invalid arguments");
    
    while (*str) {
        ESP_RETURN_ON_ERROR(lcd_write_data(handle, *str), TAG, "Failed to write character");
        str++;
    }
    
    return ESP_OK;
}

esp_err_t grove_lcd_rgb_printf(grove_lcd_rgb_handle_t handle, const char *format, ...)
{
    ESP_RETURN_ON_FALSE(handle && format, ESP_ERR_INVALID_ARG, TAG, "Invalid arguments");
    
    char buffer[64];  // Adjust size as needed
    va_list args;
    va_start(args, format);
    int len = vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);
    
    if (len < 0) {
        return ESP_ERR_INVALID_ARG;
    }
    
    return grove_lcd_rgb_print(handle, buffer);
}

esp_err_t grove_lcd_rgb_set_color(grove_lcd_rgb_handle_t handle, grove_lcd_rgb_color_t color)
{
    ESP_RETURN_ON_FALSE(handle, ESP_ERR_INVALID_ARG, TAG, "Invalid handle");
    
    ESP_RETURN_ON_ERROR(rgb_write_reg(handle, REG_RED, color.red), TAG, "Failed to set red");
    ESP_RETURN_ON_ERROR(rgb_write_reg(handle, REG_GREEN, color.green), TAG, "Failed to set green");
    ESP_RETURN_ON_ERROR(rgb_write_reg(handle, REG_BLUE, color.blue), TAG, "Failed to set blue");
    
    return ESP_OK;
}

esp_err_t grove_lcd_rgb_set_color_rgb(grove_lcd_rgb_handle_t handle, uint8_t red, uint8_t green, uint8_t blue)
{
    grove_lcd_rgb_color_t color = {red, green, blue};
    return grove_lcd_rgb_set_color(handle, color);
}

esp_err_t grove_lcd_rgb_display(grove_lcd_rgb_handle_t handle, bool on)
{
    ESP_RETURN_ON_FALSE(handle, ESP_ERR_INVALID_ARG, TAG, "Invalid handle");
    
    if (on) {
        handle->display_control |= LCD_DISPLAYON;
    } else {
        handle->display_control &= ~LCD_DISPLAYON;
    }
    
    ESP_RETURN_ON_ERROR(lcd_write_cmd(handle, handle->display_control), TAG, "Failed to set display");
    return ESP_OK;
}

esp_err_t grove_lcd_rgb_cursor(grove_lcd_rgb_handle_t handle, bool on)
{
    ESP_RETURN_ON_FALSE(handle, ESP_ERR_INVALID_ARG, TAG, "Invalid handle");
    
    if (on) {
        handle->display_control |= LCD_CURSORON;
    } else {
        handle->display_control &= ~LCD_CURSORON;
    }
    
    ESP_RETURN_ON_ERROR(lcd_write_cmd(handle, handle->display_control), TAG, "Failed to set cursor");
    return ESP_OK;
}

esp_err_t grove_lcd_rgb_blink(grove_lcd_rgb_handle_t handle, bool on)
{
    ESP_RETURN_ON_FALSE(handle, ESP_ERR_INVALID_ARG, TAG, "Invalid handle");
    
    if (on) {
        handle->display_control |= LCD_BLINKON;
    } else {
        handle->display_control &= ~LCD_BLINKON;
    }
    
    ESP_RETURN_ON_ERROR(lcd_write_cmd(handle, handle->display_control), TAG, "Failed to set blink");
    return ESP_OK;
}

esp_err_t grove_lcd_rgb_home(grove_lcd_rgb_handle_t handle)
{
    ESP_RETURN_ON_FALSE(handle, ESP_ERR_INVALID_ARG, TAG, "Invalid handle");
    
    ESP_RETURN_ON_ERROR(lcd_write_cmd(handle, LCD_RETURNHOME), TAG, "Failed to home cursor");
    vTaskDelay(pdMS_TO_TICKS(2));  // Home command needs more time
    return ESP_OK;
}