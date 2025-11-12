#ifndef XPT2046_H
#define XPT2046_H

#include "driver/spi_master.h"
#include "driver/gpio.h"

// XPT2046 Control byte commands
#define XPT2046_START_BIT   0x80

// Channel selection
#define XPT2046_X_POSITION  0x50  // Single-ended, measure X
#define XPT2046_Y_POSITION  0x10  // Single-ended, measure Y
#define XPT2046_Z1_POSITION 0x30  // Single-ended, measure Z1
#define XPT2046_Z2_POSITION 0x40  // Single-ended, measure Z2

// Differential mode
#define XPT2046_DIFF_X      0xD0  // Differential mode X
#define XPT2046_DIFF_Y      0x90  // Differential mode Y

// Power down modes
#define XPT2046_PD_IRQ_ON   0x00  // Power down, IRQ enabled
#define XPT2046_PD_IRQ_OFF  0x01  // Power down, IRQ disabled

// Touch threshold (adjust based on your screen)
#define XPT2046_TOUCH_THRESHOLD 400

// Configuration structure
typedef struct {
    spi_device_handle_t spi;
    gpio_num_t cs_pin;
    gpio_num_t irq_pin;
    uint16_t x_min;
    uint16_t x_max;
    uint16_t y_min;
    uint16_t y_max;
    uint16_t screen_width;
    uint16_t screen_height;
} xpt2046_t;

// Touch data structure
typedef struct {
    uint16_t x_raw;
    uint16_t y_raw;
    uint16_t z_raw;
    uint16_t x_calibrated;
    uint16_t y_calibrated;
    bool touched;
} xpt2046_touch_t;

// Function prototypes
esp_err_t xpt2046_init(xpt2046_t *dev, spi_host_device_t spi_host, 
                       gpio_num_t miso, gpio_num_t mosi, 
                       gpio_num_t sclk, gpio_num_t cs, gpio_num_t irq);
esp_err_t xpt2046_read_touch(xpt2046_t *dev, xpt2046_touch_t *touch_data);
uint16_t xpt2046_read_channel(xpt2046_t *dev, uint8_t command);
bool xpt2046_is_touched(xpt2046_t *dev);
void xpt2046_calibrate(xpt2046_t *dev, uint16_t x_min, uint16_t x_max, 
                       uint16_t y_min, uint16_t y_max);
esp_err_t xpt2046_deinit(xpt2046_t *dev);

#endif // XPT2046_H
