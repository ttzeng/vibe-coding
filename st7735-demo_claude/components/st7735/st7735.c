// components/st7735/st7735.c
#include "st7735.h"
#include "st7735_font.h"
#include "driver/gpio.h"
#include "driver/spi_master.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <string.h>
#include <math.h>

static const char *TAG = "ST7735";

// ST7735 Commands
#define ST7735_NOP     0x00
#define ST7735_SWRESET 0x01
#define ST7735_RDDID   0x04
#define ST7735_RDDST   0x09
#define ST7735_SLPIN   0x10
#define ST7735_SLPOUT  0x11
#define ST7735_PTLON   0x12
#define ST7735_NORON   0x13
#define ST7735_INVOFF  0x20
#define ST7735_INVON   0x21
#define ST7735_DISPOFF 0x28
#define ST7735_DISPON  0x29
#define ST7735_CASET   0x2A
#define ST7735_RASET   0x2B
#define ST7735_RAMWR   0x2C
#define ST7735_RAMRD   0x2E
#define ST7735_PTLAR   0x30
#define ST7735_COLMOD  0x3A
#define ST7735_MADCTL  0x36
#define ST7735_FRMCTR1 0xB1
#define ST7735_FRMCTR2 0xB2
#define ST7735_FRMCTR3 0xB3
#define ST7735_INVCTR  0xB4
#define ST7735_DISSET5 0xB6
#define ST7735_PWCTR1  0xC0
#define ST7735_PWCTR2  0xC1
#define ST7735_PWCTR3  0xC2
#define ST7735_PWCTR4  0xC3
#define ST7735_PWCTR5  0xC4
#define ST7735_VMCTR1  0xC5
#define ST7735_RDID1   0xDA
#define ST7735_RDID2   0xDB
#define ST7735_RDID3   0xDC
#define ST7735_RDID4   0xDD
#define ST7735_PWCTR6  0xFC
#define ST7735_GMCTRP1 0xE0
#define ST7735_GMCTRN1 0xE1

// Helper functions
static esp_err_t st7735_write_command(st7735_handle_t *handle, uint8_t cmd);
static esp_err_t st7735_write_data(st7735_handle_t *handle, const uint8_t *data, int len);
static esp_err_t st7735_write_data_byte(st7735_handle_t *handle, uint8_t data);
static esp_err_t st7735_set_addr_window(st7735_handle_t *handle, int x, int y, int w, int h);

static esp_err_t st7735_write_command(st7735_handle_t *handle, uint8_t cmd)
{
    esp_err_t ret;
    spi_transaction_t t;

    memset(&t, 0, sizeof(t));
    t.length = 8;
    t.tx_buffer = &cmd;

    gpio_set_level(handle->pin_dc, 0); // Command mode
    ret = spi_device_polling_transmit(handle->spi, &t);

    return ret;
}

static esp_err_t st7735_write_data(st7735_handle_t *handle, const uint8_t *data, int len)
{
    esp_err_t ret;
    spi_transaction_t t;

    if (len == 0) return ESP_OK;

    memset(&t, 0, sizeof(t));
    t.length = len * 8;
    t.tx_buffer = data;

    gpio_set_level(handle->pin_dc, 1); // Data mode
    ret = spi_device_polling_transmit(handle->spi, &t);

    return ret;
}

static esp_err_t st7735_write_data_byte(st7735_handle_t *handle, uint8_t data)
{
    return st7735_write_data(handle, &data, 1);
}

static esp_err_t st7735_set_addr_window(st7735_handle_t *handle, int x, int y, int w, int h)
{
    esp_err_t ret;
    uint8_t data[4];

    // Column address set
    ret = st7735_write_command(handle, ST7735_CASET);
    if (ret != ESP_OK) return ret;

    data[0] = (x >> 8) & 0xFF;
    data[1] = x & 0xFF;
    data[2] = ((x + w - 1) >> 8) & 0xFF;
    data[3] = (x + w - 1) & 0xFF;
    ret = st7735_write_data(handle, data, 4);
    if (ret != ESP_OK) return ret;

    // Row address set
    ret = st7735_write_command(handle, ST7735_RASET);
    if (ret != ESP_OK) return ret;

    data[0] = (y >> 8) & 0xFF;
    data[1] = y & 0xFF;
    data[2] = ((y + h - 1) >> 8) & 0xFF;
    data[3] = (y + h - 1) & 0xFF;
    ret = st7735_write_data(handle, data, 4);
    if (ret != ESP_OK) return ret;

    // Memory write
    ret = st7735_write_command(handle, ST7735_RAMWR);

    return ret;
}

esp_err_t st7735_init(st7735_handle_t *handle, const st7735_config_t *config)
{
    esp_err_t ret;
    
    // Initialize GPIO pins
    gpio_config_t io_conf = {
        .intr_type = GPIO_INTR_DISABLE,
        .mode = GPIO_MODE_OUTPUT,
        .pin_bit_mask = (1ULL << config->pin_dc) | (1ULL << config->pin_reset),
        .pull_down_en = 0,
        .pull_up_en = 0,
    };
    gpio_config(&io_conf);

    // Configure SPI bus
    spi_bus_config_t buscfg = {
        .miso_io_num = -1,
        .mosi_io_num = config->pin_mosi,
        .sclk_io_num = config->pin_sck,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = ST7735_WIDTH * ST7735_HEIGHT * 2,
    };

    ret = spi_bus_initialize(config->spi_host, &buscfg, SPI_DMA_CH_AUTO);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to initialize SPI bus");
        return ret;
    }

    // Configure SPI device
    spi_device_interface_config_t devcfg = {
        .clock_speed_hz = 26 * 1000 * 1000, // 26 MHz
        .mode = 0,
        .spics_io_num = config->pin_cs,
        .queue_size = 7,
        .flags = SPI_DEVICE_NO_DUMMY,
    };

    ret = spi_bus_add_device(config->spi_host, &devcfg, &handle->spi);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to add SPI device");
        spi_bus_free(config->spi_host);
        return ret;
    }

    handle->pin_dc = config->pin_dc;
    handle->pin_reset = config->pin_reset;
    handle->width = ST7735_WIDTH;
    handle->height = ST7735_HEIGHT;

    // Hardware reset
    gpio_set_level(handle->pin_reset, 0);
    vTaskDelay(pdMS_TO_TICKS(50));
    gpio_set_level(handle->pin_reset, 1);
    vTaskDelay(pdMS_TO_TICKS(50));

    // Software reset
    st7735_write_command(handle, ST7735_SWRESET);
    vTaskDelay(pdMS_TO_TICKS(150));

    // Sleep out
    st7735_write_command(handle, ST7735_SLPOUT);
    vTaskDelay(pdMS_TO_TICKS(500));

    // Frame rate control
    st7735_write_command(handle, ST7735_FRMCTR1);
    st7735_write_data_byte(handle, 0x01);
    st7735_write_data_byte(handle, 0x2C);
    st7735_write_data_byte(handle, 0x2D);

    st7735_write_command(handle, ST7735_FRMCTR2);
    st7735_write_data_byte(handle, 0x01);
    st7735_write_data_byte(handle, 0x2C);
    st7735_write_data_byte(handle, 0x2D);

    st7735_write_command(handle, ST7735_FRMCTR3);
    st7735_write_data_byte(handle, 0x01);
    st7735_write_data_byte(handle, 0x2C);
    st7735_write_data_byte(handle, 0x2D);
    st7735_write_data_byte(handle, 0x01);
    st7735_write_data_byte(handle, 0x2C);
    st7735_write_data_byte(handle, 0x2D);

    // Column inversion control
    st7735_write_command(handle, ST7735_INVCTR);
    st7735_write_data_byte(handle, 0x07);

    // Power control
    st7735_write_command(handle, ST7735_PWCTR1);
    st7735_write_data_byte(handle, 0xA2);
    st7735_write_data_byte(handle, 0x02);
    st7735_write_data_byte(handle, 0x84);

    st7735_write_command(handle, ST7735_PWCTR2);
    st7735_write_data_byte(handle, 0xC5);

    st7735_write_command(handle, ST7735_PWCTR3);
    st7735_write_data_byte(handle, 0x0A);
    st7735_write_data_byte(handle, 0x00);

    st7735_write_command(handle, ST7735_PWCTR4);
    st7735_write_data_byte(handle, 0x8A);
    st7735_write_data_byte(handle, 0x2A);

    st7735_write_command(handle, ST7735_PWCTR5);
    st7735_write_data_byte(handle, 0x8A);
    st7735_write_data_byte(handle, 0xEE);

    // VCOM control
    st7735_write_command(handle, ST7735_VMCTR1);
    st7735_write_data_byte(handle, 0x0E);

    // Memory access control
    st7735_write_command(handle, ST7735_MADCTL);
    st7735_write_data_byte(handle, 0xC8);

    // Color mode
    st7735_write_command(handle, ST7735_COLMOD);
    st7735_write_data_byte(handle, 0x05); // 16-bit color

    // Gamma correction
    st7735_write_command(handle, ST7735_GMCTRP1);
    uint8_t gmctrp1[] = {0x02, 0x1c, 0x07, 0x12, 0x37, 0x32, 0x29, 0x2d,
                         0x29, 0x25, 0x2b, 0x39, 0x00, 0x01, 0x03, 0x10};
    st7735_write_data(handle, gmctrp1, sizeof(gmctrp1));

    st7735_write_command(handle, ST7735_GMCTRN1);
    uint8_t gmctrn1[] = {0x03, 0x1d, 0x07, 0x06, 0x2e, 0x2c, 0x29, 0x2d,
                         0x2e, 0x2e, 0x37, 0x3f, 0x00, 0x00, 0x02, 0x10};
    st7735_write_data(handle, gmctrn1, sizeof(gmctrn1));

    // Normal display on
    st7735_write_command(handle, ST7735_NORON);
    vTaskDelay(pdMS_TO_TICKS(10));

    // Display on
    st7735_write_command(handle, ST7735_DISPON);
    vTaskDelay(pdMS_TO_TICKS(100));

    ESP_LOGI(TAG, "ST7735 initialized successfully");
    return ESP_OK;
}

esp_err_t st7735_deinit(st7735_handle_t *handle)
{
    if (handle->spi) {
        spi_bus_remove_device(handle->spi);
        spi_bus_free(SPI2_HOST);
    }
    return ESP_OK;
}

esp_err_t st7735_fill_screen(st7735_handle_t *handle, uint16_t color)
{
    return st7735_fill_rect(handle, 0, 0, handle->width, handle->height, color);
}

esp_err_t st7735_set_pixel(st7735_handle_t *handle, int x, int y, uint16_t color)
{
    if (x < 0 || x >= handle->width || y < 0 || y >= handle->height) {
        return ESP_ERR_INVALID_ARG;
    }

    esp_err_t ret = st7735_set_addr_window(handle, x, y, 1, 1);
    if (ret != ESP_OK) return ret;
    
    uint8_t data[2] = {color >> 8, color & 0xFF};
    return st7735_write_data(handle, data, 2);
}

esp_err_t st7735_draw_hline(st7735_handle_t *handle, int x, int y, int w, uint16_t color)
{
    if (x >= handle->width || y < 0 || y >= handle->height) {
        return ESP_ERR_INVALID_ARG;
    }

    if (x < 0) {
        w += x;
        x = 0;
    }
    if (x + w > handle->width) {
        w = handle->width - x;
    }
    if (w <= 0) return ESP_OK;
    
    esp_err_t ret = st7735_set_addr_window(handle, x, y, w, 1);
    if (ret != ESP_OK) return ret;

    uint8_t color_bytes[2] = {color >> 8, color & 0xFF};
    for (int i = 0; i < w; i++) {
        ret = st7735_write_data(handle, color_bytes, 2);
        if (ret != ESP_OK) return ret;
    }

    return ESP_OK;
}

esp_err_t st7735_draw_vline(st7735_handle_t *handle, int x, int y, int h, uint16_t color)
{
    if (x < 0 || x >= handle->width || y >= handle->height) {
        return ESP_ERR_INVALID_ARG;
    }

    if (y < 0) {
        h += y;
        y = 0;
    }
    if (y + h > handle->height) {
        h = handle->height - y;
    }
    if (h <= 0) return ESP_OK;

    esp_err_t ret = st7735_set_addr_window(handle, x, y, 1, h);
    if (ret != ESP_OK) return ret;
    
    uint8_t color_bytes[2] = {color >> 8, color & 0xFF};
    for (int i = 0; i < h; i++) {
        ret = st7735_write_data(handle, color_bytes, 2);
        if (ret != ESP_OK) return ret;
    }

    return ESP_OK;
}

esp_err_t st7735_draw_rect(st7735_handle_t *handle, int x, int y, int w, int h, uint16_t color)
{
    esp_err_t ret;
    ret = st7735_draw_hline(handle, x, y, w, color);
    if (ret != ESP_OK) return ret;
    ret = st7735_draw_hline(handle, x, y + h - 1, w, color);
    if (ret != ESP_OK) return ret;
    ret = st7735_draw_vline(handle, x, y, h, color);
    if (ret != ESP_OK) return ret;
    ret = st7735_draw_vline(handle, x + w - 1, y, h, color);
    return ret;
}

esp_err_t st7735_fill_rect(st7735_handle_t *handle, int x, int y, int w, int h, uint16_t color)
{
    if (x >= handle->width || y >= handle->height) {
        return ESP_ERR_INVALID_ARG;
    }

    if (x < 0) {
        w += x;
        x = 0;
    }
    if (y < 0) {
        h += y;
        y = 0;
    }
    if (x + w > handle->width) {
        w = handle->width - x;
    }
    if (y + h > handle->height) {
        h = handle->height - y;
    }
    if (w <= 0 || h <= 0) return ESP_OK;

    esp_err_t ret = st7735_set_addr_window(handle, x, y, w, h);
    if (ret != ESP_OK) return ret;

    uint8_t color_bytes[2] = {color >> 8, color & 0xFF};
    int pixels = w * h;

    for (int i = 0; i < pixels; i++) {
        ret = st7735_write_data(handle, color_bytes, 2);
        if (ret != ESP_OK) return ret;
    }

    return ESP_OK;
}

esp_err_t st7735_draw_circle(st7735_handle_t *handle, int x0, int y0, int r, uint16_t color)
{
    int x = r;
    int y = 0;
    int err = 0;

    while (x >= y) {
        st7735_set_pixel(handle, x0 + x, y0 + y, color);
        st7735_set_pixel(handle, x0 + y, y0 + x, color);
        st7735_set_pixel(handle, x0 - y, y0 + x, color);
        st7735_set_pixel(handle, x0 - x, y0 + y, color);
        st7735_set_pixel(handle, x0 - x, y0 - y, color);
        st7735_set_pixel(handle, x0 - y, y0 - x, color);
        st7735_set_pixel(handle, x0 + y, y0 - x, color);
        st7735_set_pixel(handle, x0 + x, y0 - y, color);

        if (err <= 0) {
            y += 1;
            err += 2*y + 1;
        }
        if (err > 0) {
            x -= 1;
            err -= 2*x + 1;
        }
    }

    return ESP_OK;
}

esp_err_t st7735_fill_circle(st7735_handle_t *handle, int x0, int y0, int r, uint16_t color)
{
    for (int y = -r; y <= r; y++) {
        for (int x = -r; x <= r; x++) {
            if (x*x + y*y <= r*r) {
                st7735_set_pixel(handle, x0 + x, y0 + y, color);
            }
        }
    }
    return ESP_OK;
}

uint16_t st7735_rgb_to_565(uint8_t r, uint8_t g, uint8_t b)
{
    return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
}

esp_err_t st7735_draw_char(st7735_handle_t *handle, int x, int y, char c, uint16_t color, uint16_t bg_color, uint8_t size)
{
    if (c < 32 || c > 127) {
        c = '?'; // Replace unsupported characters with question mark
    }

    const uint8_t *char_data = font5x8[c - 32];

    for (int i = 0; i < FONT_WIDTH; i++) {
        uint8_t column = char_data[i];
        for (int j = 0; j < FONT_HEIGHT; j++) {
            if (column & (1 << j)) {
                // Draw foreground pixel(s)
                if (size == 1) {
                    st7735_set_pixel(handle, x + i, y + j, color);
                } else {
                    st7735_fill_rect(handle, x + i * size, y + j * size, size, size, color);
                }
            } else if (bg_color != color) {
                // Draw background pixel(s) if background color is different
                if (size == 1) {
                    st7735_set_pixel(handle, x + i, y + j, bg_color);
                } else {
                    st7735_fill_rect(handle, x + i * size, y + j * size, size, size, bg_color);
                }
            }
        }
    }

    // Add spacing between characters
    if (bg_color != color) {
        for (int j = 0; j < FONT_HEIGHT; j++) {
            if (size == 1) {
                st7735_set_pixel(handle, x + FONT_WIDTH, y + j, bg_color);
            } else {
                st7735_fill_rect(handle, x + FONT_WIDTH * size, y + j * size, size, size, bg_color);
            }
        }
    }

    return ESP_OK;
}

esp_err_t st7735_draw_string(st7735_handle_t *handle, int x, int y, const char *str, uint16_t color, uint16_t bg_color, uint8_t size)
{
    if (!str) return ESP_ERR_INVALID_ARG;

    int cur_x = x;
    int cur_y = y;

    while (*str) {
        if (*str == '\n') {
            cur_x = x;
            cur_y += (FONT_HEIGHT + 1) * size;
            str++;
            continue;
        }

        if (*str == '\r') {
            cur_x = x;
            str++;
            continue;
        }

        // Check if character fits on current line
        if (cur_x + (FONT_WIDTH + 1) * size > handle->width) {
            cur_x = x;
            cur_y += (FONT_HEIGHT + 1) * size;
        }

        // Check if we're still within display bounds
        if (cur_y + FONT_HEIGHT * size > handle->height) {
            break; // Stop if we go beyond display
        }

        st7735_draw_char(handle, cur_x, cur_y, *str, color, bg_color, size);
        cur_x += (FONT_WIDTH + 1) * size;
        str++;
    }

    return ESP_OK;
}

int st7735_get_text_width(const char *str, uint8_t size)
{
    if (!str) return 0;

    int line_width = 0;
    int max_width = 0;

    while (*str) {
        if (*str == '\n' || *str == '\r') {
            if (line_width > max_width) {
                max_width = line_width;
            }
            line_width = 0;
        } else {
            line_width += (FONT_WIDTH + 1) * size;
        }
        str++;
    }

    if (line_width > max_width) {
        max_width = line_width;
    }

    return max_width - size; // Subtract the last character spacing
}

int st7735_get_text_height(uint8_t size)
{
    return FONT_HEIGHT * size;
}

esp_err_t st7735_set_cursor(st7735_handle_t *handle, int x, int y)
{
    handle->cursor.x = x;
    handle->cursor.y = y;
    return ESP_OK;
}

esp_err_t st7735_printf(st7735_handle_t *handle, uint16_t color, uint16_t bg_color, uint8_t size, const char *format, ...)
{
    char buffer[256]; // Adjust size as needed
    va_list args;

    va_start(args, format);
    int len = vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);

    if (len < 0) {
        return ESP_ERR_INVALID_ARG;
    }

    esp_err_t ret = st7735_draw_string(handle, handle->cursor.x, handle->cursor.y, buffer, color, bg_color, size);

    // Update cursor position after printing
    if (ret == ESP_OK) {
        // Find the last line position
        int lines = 1;
        int last_line_chars = 0;
        for (int i = 0; i < len && buffer[i] != '\0'; i++) {
            if (buffer[i] == '\n') {
                lines++;
                last_line_chars = 0;
            } else if (buffer[i] != '\r') {
                last_line_chars++;
            }
        }

        handle->cursor.x += last_line_chars * (FONT_WIDTH + 1) * size;
        handle->cursor.y += (lines - 1) * (FONT_HEIGHT + 1) * size;
    }

    return ret;
}
