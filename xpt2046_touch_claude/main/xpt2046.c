#include <string.h>
#include "xpt2046.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static const char *TAG = "XPT2046";

// SPI clock speed (typically 1-2 MHz for XPT2046)
#define XPT2046_SPI_CLOCK_HZ (2 * 1000 * 1000)

/**
 * Initialize the XPT2046 touch controller
 */
esp_err_t xpt2046_init(xpt2046_t *dev, spi_host_device_t spi_host,
                       gpio_num_t miso, gpio_num_t mosi,
                       gpio_num_t sclk, gpio_num_t cs, gpio_num_t irq)
{
    esp_err_t ret;

    // Configure SPI bus
    spi_bus_config_t buscfg = {
        .miso_io_num = miso,
        .mosi_io_num = mosi,
        .sclk_io_num = sclk,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = 32,
    };

    // Initialize SPI bus
    ret = spi_bus_initialize(spi_host, &buscfg, SPI_DMA_CH_AUTO);
    if (ret != ESP_OK && ret != ESP_ERR_INVALID_STATE) {
        ESP_LOGE(TAG, "Failed to initialize SPI bus: %s", esp_err_to_name(ret));
        return ret;
    }

    // Configure SPI device
    spi_device_interface_config_t devcfg = {
        .clock_speed_hz = XPT2046_SPI_CLOCK_HZ,
        .mode = 0,                          // SPI mode 0
        .spics_io_num = cs,
        .queue_size = 1,
        .pre_cb = NULL,
        .post_cb = NULL,
        .command_bits = 0,
        .address_bits = 0,
        .dummy_bits = 0,
    };

    // Attach the XPT2046 to the SPI bus
    ret = spi_bus_add_device(spi_host, &devcfg, &dev->spi);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to add SPI device: %s", esp_err_to_name(ret));
        return ret;
    }

    dev->cs_pin = cs;
    dev->irq_pin = irq;

    // Configure IRQ pin if provided
    if (irq != GPIO_NUM_NC) {
        gpio_config_t io_conf = {
            .pin_bit_mask = (1ULL << irq),
            .mode = GPIO_MODE_INPUT,
            .pull_up_en = GPIO_PULLUP_ENABLE,
            .pull_down_en = GPIO_PULLDOWN_DISABLE,
            .intr_type = GPIO_INTR_DISABLE,
        };
        gpio_config(&io_conf);
    }

    // Set default calibration values (raw ADC range)
    dev->x_min = 200;
    dev->x_max = 3900;
    dev->y_min = 200;
    dev->y_max = 3900;
    dev->screen_width = 320;
    dev->screen_height = 240;

    ESP_LOGI(TAG, "XPT2046 initialized successfully");
    return ESP_OK;
}

/**
 * Read a single channel from XPT2046
 */
uint16_t xpt2046_read_channel(xpt2046_t *dev, uint8_t command)
{
    esp_err_t ret;
    spi_transaction_t t;
    uint8_t tx_data[3] = {0};
    uint8_t rx_data[3] = {0};

    // Construct command byte
    tx_data[0] = XPT2046_START_BIT | command;
    tx_data[1] = 0x00;
    tx_data[2] = 0x00;

    memset(&t, 0, sizeof(t));
    t.length = 24;  // 3 bytes = 24 bits
    t.tx_buffer = tx_data;
    t.rx_buffer = rx_data;

    ret = spi_device_polling_transmit(dev->spi, &t);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "SPI transmission failed: %s", esp_err_to_name(ret));
        return 0;
    }

    // XPT2046 returns 12-bit data in bits [14:3] of the response
    uint16_t result = ((rx_data[1] << 8) | rx_data[2]) >> 3;
    
    return result & 0x0FFF;  // Mask to 12 bits
}

/**
 * Check if the touch screen is currently touched
 */
bool xpt2046_is_touched(xpt2046_t *dev)
{
    if (dev->irq_pin == GPIO_NUM_NC) {
        // If no IRQ pin, read Z position to detect touch
        uint16_t z = xpt2046_read_channel(dev, XPT2046_Z1_POSITION);
        return z > XPT2046_TOUCH_THRESHOLD;
    }
    
    // IRQ pin is LOW when touched (active low)
    return gpio_get_level(dev->irq_pin) == 0;
}

/**
 * Read touch position with averaging
 */
esp_err_t xpt2046_read_touch(xpt2046_t *dev, xpt2046_touch_t *touch_data)
{
    if (dev == NULL || touch_data == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    memset(touch_data, 0, sizeof(xpt2046_touch_t));

    // Check if touched
    if (!xpt2046_is_touched(dev)) {
        touch_data->touched = false;
        return ESP_OK;
    }

    // Small delay to stabilize
    vTaskDelay(pdMS_TO_TICKS(1));

    // Read multiple samples and average for better accuracy
    const int samples = 8;
    uint32_t x_sum = 0, y_sum = 0, z_sum = 0;
    int valid_samples = 0;

    for (int i = 0; i < samples; i++) {
        uint16_t x = xpt2046_read_channel(dev, XPT2046_DIFF_X);
        uint16_t y = xpt2046_read_channel(dev, XPT2046_DIFF_Y);
        uint16_t z1 = xpt2046_read_channel(dev, XPT2046_Z1_POSITION);

        // Filter out invalid readings
        if (x > 100 && x < 4000 && y > 100 && y < 4000) {
            x_sum += x;
            y_sum += y;
            z_sum += z1;
            valid_samples++;
        }
        
        vTaskDelay(pdMS_TO_TICKS(1));
    }

    if (valid_samples == 0) {
        touch_data->touched = false;
        return ESP_OK;
    }

    // Calculate averages
    touch_data->x_raw = x_sum / valid_samples;
    touch_data->y_raw = y_sum / valid_samples;
    touch_data->z_raw = z_sum / valid_samples;

    // Apply calibration
    if (touch_data->x_raw < dev->x_min) touch_data->x_raw = dev->x_min;
    if (touch_data->x_raw > dev->x_max) touch_data->x_raw = dev->x_max;
    if (touch_data->y_raw < dev->y_min) touch_data->y_raw = dev->y_min;
    if (touch_data->y_raw > dev->y_max) touch_data->y_raw = dev->y_max;

    // Map to screen coordinates
    touch_data->x_calibrated = ((touch_data->x_raw - dev->x_min) * dev->screen_width) / 
                               (dev->x_max - dev->x_min);
    touch_data->y_calibrated = ((touch_data->y_raw - dev->y_min) * dev->screen_height) / 
                               (dev->y_max - dev->y_min);

    touch_data->touched = true;

    ESP_LOGD(TAG, "Touch: Raw(%d, %d) Z=%d, Cal(%d, %d)",
             touch_data->x_raw, touch_data->y_raw, touch_data->z_raw,
             touch_data->x_calibrated, touch_data->y_calibrated);

    return ESP_OK;
}

/**
 * Set calibration parameters
 */
void xpt2046_calibrate(xpt2046_t *dev, uint16_t x_min, uint16_t x_max,
                       uint16_t y_min, uint16_t y_max)
{
    dev->x_min = x_min;
    dev->x_max = x_max;
    dev->y_min = y_min;
    dev->y_max = y_max;
    
    ESP_LOGI(TAG, "Calibration set: X[%d-%d], Y[%d-%d]", 
             x_min, x_max, y_min, y_max);
}

/**
 * Deinitialize XPT2046
 */
esp_err_t xpt2046_deinit(xpt2046_t *dev)
{
    if (dev == NULL || dev->spi == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    esp_err_t ret = spi_bus_remove_device(dev->spi);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to remove SPI device: %s", esp_err_to_name(ret));
        return ret;
    }

    dev->spi = NULL;
    ESP_LOGI(TAG, "XPT2046 deinitialized");
    
    return ESP_OK;
}
