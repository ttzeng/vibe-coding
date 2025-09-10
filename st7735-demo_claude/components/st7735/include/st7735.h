// components/st7735/include/st7735.h
#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "esp_err.h"
#include "driver/spi_master.h"

#ifdef __cplusplus
extern "C" {
#endif

// Display dimensions
#define ST7735_WIDTH  128
#define ST7735_HEIGHT 160

// Colors (RGB565)
#define ST7735_BLACK   0x0000
#define ST7735_BLUE    0x001F
#define ST7735_RED     0xF800
#define ST7735_GREEN   0x07E0
#define ST7735_CYAN    0x07FF
#define ST7735_MAGENTA 0xF81F
#define ST7735_YELLOW  0xFFE0
#define ST7735_WHITE   0xFFFF

// ST7735 Configuration
typedef struct {
    int pin_mosi;
    int pin_sck;
    int pin_cs;
    int pin_dc;
    int pin_reset;
    spi_host_device_t spi_host;
} st7735_config_t;

// Text cursor structure
typedef struct {
    int x;
    int y;
    uint16_t color;
    uint16_t bg_color;
    uint8_t size;
} st7735_cursor_t;

// ST7735 Handle
typedef struct {
    spi_device_handle_t spi;
    int pin_dc;
    int pin_reset;
    uint16_t width;
    uint16_t height;
    st7735_cursor_t cursor;
} st7735_handle_t;

/**
 * @brief Initialize ST7735 display
 */
esp_err_t st7735_init(st7735_handle_t *handle, const st7735_config_t *config);

/**
 * @brief Deinitialize ST7735 display
 */
esp_err_t st7735_deinit(st7735_handle_t *handle);

/**
 * @brief Fill entire screen with color
 */
esp_err_t st7735_fill_screen(st7735_handle_t *handle, uint16_t color);

/**
 * @brief Set pixel at coordinates
 */
esp_err_t st7735_set_pixel(st7735_handle_t *handle, int x, int y, uint16_t color);

/**
 * @brief Draw horizontal line
 */
esp_err_t st7735_draw_hline(st7735_handle_t *handle, int x, int y, int w, uint16_t color);

/**
 * @brief Draw vertical line
 */
esp_err_t st7735_draw_vline(st7735_handle_t *handle, int x, int y, int h, uint16_t color);

/**
 * @brief Draw rectangle
 */
esp_err_t st7735_draw_rect(st7735_handle_t *handle, int x, int y, int w, int h, uint16_t color);

/**
 * @brief Fill rectangle
 */
esp_err_t st7735_fill_rect(st7735_handle_t *handle, int x, int y, int w, int h, uint16_t color);

/**
 * @brief Draw circle
 */
esp_err_t st7735_draw_circle(st7735_handle_t *handle, int x0, int y0, int r, uint16_t color);

/**
 * @brief Fill circle
 */
esp_err_t st7735_fill_circle(st7735_handle_t *handle, int x0, int y0, int r, uint16_t color);

/**
 * @brief Convert RGB888 to RGB565
 */
uint16_t st7735_rgb_to_565(uint8_t r, uint8_t g, uint8_t b);

/**
 * @brief Draw a single character
 */
esp_err_t st7735_draw_char(st7735_handle_t *handle, int x, int y, char c, uint16_t color, uint16_t bg_color, uint8_t size);

/**
 * @brief Draw a string of text
 */
esp_err_t st7735_draw_string(st7735_handle_t *handle, int x, int y, const char *str, uint16_t color, uint16_t bg_color, uint8_t size);

/**
 * @brief Get text width in pixels
 */
int st7735_get_text_width(const char *str, uint8_t size);

/**
 * @brief Get text height in pixels
 */
int st7735_get_text_height(uint8_t size);

/**
 * @brief Set text cursor position
 */
esp_err_t st7735_set_cursor(st7735_handle_t *handle, int x, int y);

/**
 * @brief Print formatted text at cursor position
 */
esp_err_t st7735_printf(st7735_handle_t *handle, uint16_t color, uint16_t bg_color, uint8_t size, const char *format, ...);

#ifdef __cplusplus
}
#endif
