#ifndef SENSOR_MANAGER_H
#define SENSOR_MANAGER_H

#include <stdbool.h>
#include "esp_err.h"

typedef struct {
    float temperature;
    float humidity;
    float pressure;
    uint16_t light_level;
    bool data_valid;
    uint32_t last_update;
} sensor_data_t;

// Sensor Manager API
esp_err_t sensor_manager_init(void);
esp_err_t sensor_manager_update(void);
sensor_data_t* sensor_manager_get_data(void);
bool sensor_manager_is_data_valid(void);

// Individual sensor functions (for future expansion)
esp_err_t sensor_read_temperature(float *temperature);
esp_err_t sensor_read_humidity(float *humidity);
esp_err_t sensor_read_pressure(float *pressure);
esp_err_t sensor_read_light(uint16_t *light_level);

#endif // SENSOR_MANAGER_H
