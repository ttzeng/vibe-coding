#include "adxl345.h"
#include "esp_log.h"
#include <stdlib.h>
#include <string.h>

static const char *TAG = "ADXL345";

// Internal helper functions
static esp_err_t adxl345_write_reg(adxl345_handle_t *handle, uint8_t reg_addr, uint8_t data);
static esp_err_t adxl345_read_reg(adxl345_handle_t *handle, uint8_t reg_addr, uint8_t *data);
static esp_err_t adxl345_read_regs(adxl345_handle_t *handle, uint8_t reg_addr, uint8_t *data, size_t len);

esp_err_t adxl345_init(const adxl345_config_t *config, adxl345_handle_t **handle)
{
    esp_err_t ret;
    
    if (config == NULL || handle == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    // Allocate handle
    adxl345_handle_t *adxl345_handle = calloc(1, sizeof(adxl345_handle_t));
    if (adxl345_handle == NULL) {
        ESP_LOGE(TAG, "Failed to allocate memory for ADXL345 handle");
        return ESP_ERR_NO_MEM;
    }

    // Configure I2C master bus
    i2c_master_bus_config_t bus_config = {
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .i2c_port = I2C_NUM_0,
        .scl_io_num = config->scl_pin,
        .sda_io_num = config->sda_pin,
        .glitch_ignore_cnt = 7,
        .flags.enable_internal_pullup = config->enable_pullup,
    };

    ret = i2c_new_master_bus(&bus_config, &adxl345_handle->bus_handle);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to create I2C master bus: %s", esp_err_to_name(ret));
        free(adxl345_handle);
        return ret;
    }

    // Configure I2C device
    i2c_device_config_t dev_config = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address = ADXL345_I2C_ADDRESS,
        .scl_speed_hz = config->clk_speed,
    };

    ret = i2c_master_bus_add_device(adxl345_handle->bus_handle, &dev_config, &adxl345_handle->dev_handle);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to add I2C device: %s", esp_err_to_name(ret));
        i2c_del_master_bus(adxl345_handle->bus_handle);
        free(adxl345_handle);
        return ret;
    }

    // Initialize scale factor (default for ±2g range)
    adxl345_handle->scale_factor = 4.0f; // mg per LSB

    // Check device ID
    ret = adxl345_check_device_id(adxl345_handle);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Device ID check failed");
        adxl345_deinit(adxl345_handle);
        return ret;
    }

    // Set default configuration
    ret = adxl345_set_range(adxl345_handle, ADXL345_RANGE_2G);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to set range");
        adxl345_deinit(adxl345_handle);
        return ret;
    }

    ret = adxl345_set_data_rate(adxl345_handle, ADXL345_BW_RATE_100HZ);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to set data rate");
        adxl345_deinit(adxl345_handle);
        return ret;
    }

    ret = adxl345_enable_measurement(adxl345_handle);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to enable measurement");
        adxl345_deinit(adxl345_handle);
        return ret;
    }

    *handle = adxl345_handle;
    ESP_LOGI(TAG, "ADXL345 initialized successfully");
    return ESP_OK;
}

esp_err_t adxl345_deinit(adxl345_handle_t *handle)
{
    if (handle == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    esp_err_t ret = ESP_OK;

    // Remove device from bus
    if (handle->dev_handle) {
        ret = i2c_master_bus_rm_device(handle->dev_handle);
        if (ret != ESP_OK) {
            ESP_LOGE(TAG, "Failed to remove I2C device: %s", esp_err_to_name(ret));
        }
    }

    // Delete master bus
    if (handle->bus_handle) {
        esp_err_t bus_ret = i2c_del_master_bus(handle->bus_handle);
        if (bus_ret != ESP_OK) {
            ESP_LOGE(TAG, "Failed to delete I2C master bus: %s", esp_err_to_name(bus_ret));
            ret = bus_ret;
        }
    }

    // Free handle memory
    free(handle);
    ESP_LOGI(TAG, "ADXL345 deinitialized");
    return ret;
}

esp_err_t adxl345_check_device_id(adxl345_handle_t *handle)
{
    if (handle == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    uint8_t device_id;
    esp_err_t ret = adxl345_read_reg(handle, ADXL345_REG_DEVID, &device_id);
    
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to read device ID: %s", esp_err_to_name(ret));
        return ret;
    }

    if (device_id != ADXL345_DEVICE_ID) {
        ESP_LOGE(TAG, "Invalid device ID: 0x%02X (expected 0x%02X)", device_id, ADXL345_DEVICE_ID);
        return ESP_ERR_INVALID_RESPONSE;
    }

    ESP_LOGI(TAG, "Device ID verified: 0x%02X", device_id);
    return ESP_OK;
}

esp_err_t adxl345_set_range(adxl345_handle_t *handle, adxl345_range_t range)
{
    if (handle == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    uint8_t data_format = ADXL345_DATA_FORMAT_FULL_RES | range;
    esp_err_t ret = adxl345_write_reg(handle, ADXL345_REG_DATA_FORMAT, data_format);
    
    if (ret == ESP_OK) {
        // Update scale factor based on range
        switch (range) {
            case ADXL345_RANGE_2G:  handle->scale_factor = 4.0f; break;  // 4 mg/LSB
            case ADXL345_RANGE_4G:  handle->scale_factor = 8.0f; break;  // 8 mg/LSB
            case ADXL345_RANGE_8G:  handle->scale_factor = 16.0f; break; // 16 mg/LSB
            case ADXL345_RANGE_16G: handle->scale_factor = 32.0f; break; // 32 mg/LSB
        }
        ESP_LOGI(TAG, "Range set to ±%dg", 2 << range);
    }
    
    return ret;
}

esp_err_t adxl345_set_data_rate(adxl345_handle_t *handle, uint8_t rate)
{
    if (handle == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    return adxl345_write_reg(handle, ADXL345_REG_BW_RATE, rate);
}

esp_err_t adxl345_enable_measurement(adxl345_handle_t *handle)
{
    if (handle == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    return adxl345_write_reg(handle, ADXL345_REG_POWER_CTL, ADXL345_POWER_CTL_MEASURE);
}

esp_err_t adxl345_disable_measurement(adxl345_handle_t *handle)
{
    if (handle == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    return adxl345_write_reg(handle, ADXL345_REG_POWER_CTL, 0x00);
}

esp_err_t adxl345_read_raw_data(adxl345_handle_t *handle, int16_t *x, int16_t *y, int16_t *z)
{
    if (handle == NULL || x == NULL || y == NULL || z == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    uint8_t data[6];
    esp_err_t ret = adxl345_read_regs(handle, ADXL345_REG_DATAX0, data, 6);
    
    if (ret == ESP_OK) {
        *x = (int16_t)((data[1] << 8) | data[0]);
        *y = (int16_t)((data[3] << 8) | data[2]);
        *z = (int16_t)((data[5] << 8) | data[4]);
    }
    
    return ret;
}

esp_err_t adxl345_read_acceleration(adxl345_handle_t *handle, adxl345_accel_data_t *data)
{
    if (handle == NULL || data == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    int16_t raw_x, raw_y, raw_z;
    esp_err_t ret = adxl345_read_raw_data(handle, &raw_x, &raw_y, &raw_z);
    
    if (ret == ESP_OK) {
        // Convert to g (gravity units)
        data->x = (float)raw_x * handle->scale_factor / 1000.0f;
        data->y = (float)raw_y * handle->scale_factor / 1000.0f;
        data->z = (float)raw_z * handle->scale_factor / 1000.0f;
    }
    
    return ret;
}

// Internal helper functions
static esp_err_t adxl345_write_reg(adxl345_handle_t *handle, uint8_t reg_addr, uint8_t data)
{
    uint8_t write_buf[2] = {reg_addr, data};
    return i2c_master_transmit(handle->dev_handle, write_buf, sizeof(write_buf), ADXL345_I2C_TIMEOUT_MS);
}

static esp_err_t adxl345_read_reg(adxl345_handle_t *handle, uint8_t reg_addr, uint8_t *data)
{
    return i2c_master_transmit_receive(handle->dev_handle, &reg_addr, 1, data, 1, ADXL345_I2C_TIMEOUT_MS);
}

static esp_err_t adxl345_read_regs(adxl345_handle_t *handle, uint8_t reg_addr, uint8_t *data, size_t len)
{
    return i2c_master_transmit_receive(handle->dev_handle, &reg_addr, 1, data, len, ADXL345_I2C_TIMEOUT_MS);
}
