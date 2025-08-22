idf_component_register(
    SRCS "src/grove_lcd_rgb.c"
    INCLUDE_DIRS "include"
    REQUIRES driver
    PRIV_REQUIRES esp_timer
)