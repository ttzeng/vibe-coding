#ifndef SSD1306_H
#define SSD1306_H

#include "esp_err.h"
#include "driver/i2c_master.h"

#ifdef __cplusplus
extern "C" {
#endif

// SSD1306 Configuration
#define SSD1306_I2C_ADDRESS     0x3C
#define SSD1306_WIDTH           128
#define SSD1306_HEIGHT          64
#define SSD1306_BUFFER_SIZE     ((SSD1306_WIDTH * SSD1306_HEIGHT) / 8)

// Colors
#define SSD1306_COLOR_BLACK     0
#define SSD1306_COLOR_WHITE     1

// Font sizes
#define SSD1306_FONT_SIZE_11    11
#define SSD1306_FONT_SIZE_16    16

typedef struct ssd1306_dev* ssd1306_handle_t;

/**
 * @brief Create SSD1306 device handle
 * @param bus_handle I2C master bus handle
 * @param dev_addr Device I2C address
 * @return SSD1306 device handle or NULL on error
 */
ssd1306_handle_t ssd1306_create(i2c_master_bus_handle_t bus_handle, uint8_t dev_addr);

/**
 * @brief Delete SSD1306 device handle
 * @param dev SSD1306 device handle
 */
void ssd1306_delete(ssd1306_handle_t dev);

/**
 * @brief Initialize SSD1306 OLED display
 * @param dev SSD1306 device handle
 * @return ESP_OK on success
 */
esp_err_t ssd1306_init(ssd1306_handle_t dev);

/**
 * @brief Clear screen with specified pattern
 * @param dev SSD1306 device handle
 * @param chFill Fill pattern (0x00 for black, 0xFF for white)
 */
void ssd1306_clear_screen(ssd1306_handle_t dev, uint8_t chFill);

/**
 * @brief Refresh GRAM (display buffer to screen)
 * @param dev SSD1306 device handle
 */
void ssd1306_refresh_gram(ssd1306_handle_t dev);

/**
 * @brief Draw a point
 * @param dev SSD1306 device handle
 * @param chXpos X coordinate
 * @param chYpos Y coordinate
 * @param chPoint Color (0=black, 1=white)
 */
void ssd1306_draw_point(ssd1306_handle_t dev, uint8_t chXpos, uint8_t chYpos, uint8_t chPoint);

/**
 * @brief Draw a line
 * @param dev SSD1306 device handle
 * @param chXpos0 Start X coordinate
 * @param chYpos0 Start Y coordinate
 * @param chXpos1 End X coordinate
 * @param chYpos1 End Y coordinate
 * @param chMode Color (0=black, 1=white)
 */
void ssd1306_draw_line(ssd1306_handle_t dev, uint8_t chXpos0, uint8_t chYpos0, uint8_t chXpos1, uint8_t chYpos1, uint8_t chMode);

/**
 * @brief Draw a rectangle
 * @param dev SSD1306 device handle
 * @param chXpos0 Top-left X coordinate
 * @param chYpos0 Top-left Y coordinate
 * @param chWidth Rectangle width
 * @param chHeight Rectangle height
 * @param chMode Color (0=black, 1=white)
 */
void ssd1306_draw_rectangle(ssd1306_handle_t dev, uint8_t chXpos0, uint8_t chYpos0, uint8_t chWidth, uint8_t chHeight, uint8_t chMode);

/**
 * @brief Show a string on the display
 * @param dev SSD1306 device handle
 * @param chXpos X coordinate
 * @param chYpos Y coordinate
 * @param pchString String to display
 * @param chSize Font size (16 supported)
 * @param chMode Color (0=black, 1=white)
 */
void ssd1306_show_string(ssd1306_handle_t dev, uint8_t chXpos, uint8_t chYpos, const char *pchString, uint8_t chSize, uint8_t chMode);

/**
 * @brief Show a single character
 * @param dev SSD1306 device handle
 * @param chXpos X coordinate
 * @param chYpos Y coordinate
 * @param chChr Character to display
 * @param chSize Font size (16 supported)
 * @param chMode Color (0=black, 1=white)
 */
void ssd1306_show_char(ssd1306_handle_t dev, uint8_t chXpos, uint8_t chYpos, uint8_t chChr, uint8_t chSize, uint8_t chMode);

#ifdef __cplusplus
}
#endif

#endif // SSD1306_H
