#ifndef ADXL345_H
#define ADXL345_H

#include <stdint.h>
#include "driver/i2c_master.h"
#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

// ADXL345 I2C Address
#define ADXL345_I2C_ADDRESS         0x53

// ADXL345 Register Map
#define ADXL345_REG_DEVID           0x00
#define ADXL345_REG_THRESH_TAP      0x1D
#define ADXL345_REG_OFSX            0x1E
#define ADXL345_REG_OFSY            0x1F
#define ADXL345_REG_OFSZ            0x20
#define ADXL345_REG_DUR             0x21
#define ADXL345_REG_LATENT          0x22
#define ADXL345_REG_WINDOW          0x23
#define ADXL345_REG_THRESH_ACT      0x24
#define ADXL345_REG_THRESH_INACT    0x25
#define ADXL345_REG_TIME_INACT      0x26
#define ADXL345_REG_ACT_INACT_CTL   0x27
#define ADXL345_REG_THRESH_FF       0x28
#define ADXL345_REG_TIME_FF         0x29
#define ADXL345_REG_TAP_AXES        0x2A
#define ADXL345_REG_ACT_TAP_STATUS  0x2B
#define ADXL345_REG_BW_RATE         0x2C
#define ADXL345_REG_POWER_CTL       0x2D
#define ADXL345_REG_INT_ENABLE      0x2E
#define ADXL345_REG_INT_MAP         0x2F
#define ADXL345_REG_INT_SOURCE      0x30
#define ADXL345_REG_DATA_FORMAT     0x31
#define ADXL345_REG_DATAX0          0x32
#define ADXL345_REG_DATAX1          0x33
#define ADXL345_REG_DATAY0          0x34
#define ADXL345_REG_DATAY1          0x35
#define ADXL345_REG_DATAZ0          0x36
#define ADXL345_REG_DATAZ1          0x37
#define ADXL345_REG_FIFO_CTL        0x38
#define ADXL345_REG_FIFO_STATUS     0x39

// Power Control Register bits
#define ADXL345_POWER_CTL_MEASURE   0x08
#define ADXL345_POWER_CTL_SLEEP     0x04
#define ADXL345_POWER_CTL_WAKEUP    0x03

// Data Format Register bits
#define ADXL345_DATA_FORMAT_RANGE_2G    0x00
#define ADXL345_DATA_FORMAT_RANGE_4G    0x01
#define ADXL345_DATA_FORMAT_RANGE_8G    0x02
#define ADXL345_DATA_FORMAT_RANGE_16G   0x03
#define ADXL345_DATA_FORMAT_FULL_RES    0x08

// Device ID
#define ADXL345_DEVICE_ID           0xE5

// Data rate settings
#define ADXL345_BW_RATE_3200HZ      0x0F
#define ADXL345_BW_RATE_1600HZ      0x0E
#define ADXL345_BW_RATE_800HZ       0x0D
#define ADXL345_BW_RATE_400HZ       0x0C
#define ADXL345_BW_RATE_200HZ       0x0B
#define ADXL345_BW_RATE_100HZ       0x0A
#define ADXL345_BW_RATE_50HZ        0x09
#define ADXL345_BW_RATE_25HZ        0x08

// Default timeout for I2C operations
#define ADXL345_I2C_TIMEOUT_MS      1000

// Configuration structure
typedef struct {
    gpio_num_t sda_pin;
    gpio_num_t scl_pin;
    uint32_t clk_speed;
    bool enable_pullup;
} adxl345_config_t;

// ADXL345 handle structure
typedef struct {
    i2c_master_bus_handle_t bus_handle;
    i2c_master_dev_handle_t dev_handle;
    float scale_factor;
} adxl345_handle_t;

// Accelerometer data structure
typedef struct {
    float x;  // X-axis acceleration in g
    float y;  // Y-axis acceleration in g
    float z;  // Z-axis acceleration in g
} adxl345_accel_data_t;

// Range settings
typedef enum {
    ADXL345_RANGE_2G = 0,
    ADXL345_RANGE_4G,
    ADXL345_RANGE_8G,
    ADXL345_RANGE_16G
} adxl345_range_t;

// Function declarations
esp_err_t adxl345_init(const adxl345_config_t *config, adxl345_handle_t **handle);
esp_err_t adxl345_deinit(adxl345_handle_t *handle);
esp_err_t adxl345_check_device_id(adxl345_handle_t *handle);
esp_err_t adxl345_set_range(adxl345_handle_t *handle, adxl345_range_t range);
esp_err_t adxl345_set_data_rate(adxl345_handle_t *handle, uint8_t rate);
esp_err_t adxl345_enable_measurement(adxl345_handle_t *handle);
esp_err_t adxl345_disable_measurement(adxl345_handle_t *handle);
esp_err_t adxl345_read_acceleration(adxl345_handle_t *handle, adxl345_accel_data_t *data);
esp_err_t adxl345_read_raw_data(adxl345_handle_t *handle, int16_t *x, int16_t *y, int16_t *z);

#ifdef __cplusplus
}
#endif

#endif // ADXL345_H
