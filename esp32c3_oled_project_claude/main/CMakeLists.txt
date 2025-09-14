idf_component_register(
    SRCS "main.c"
         "display_manager.c"
         "menu_system.c"
         "sensor_manager.c"
         "wifi_manager.c"
    INCLUDE_DIRS "."
    REQUIRES ssd1306 animations utils nvs_flash esp_wifi esp_netif
)
