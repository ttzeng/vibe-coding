idf_component_register(SRCS "esp32_led_fade.c"
                       PRIV_REQUIRES esp_driver_gpio esp_driver_uart esp_driver_ledc
                       INCLUDE_DIRS "")
