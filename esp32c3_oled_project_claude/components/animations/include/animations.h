#ifndef ANIMATIONS_H
#define ANIMATIONS_H

#include "ssd1306.h"
#include "app_config.h"

// Animation API
esp_err_t animations_init(void);
void animations_reset(void);
void animations_set_type(animation_type_t type);
void animations_update(ssd1306_handle_t display, uint32_t frame);

#endif // ANIMATIONS_H
