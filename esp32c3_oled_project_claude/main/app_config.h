#ifndef APP_CONFIG_H
#define APP_CONFIG_H

// Hardware Configuration
#define I2C_MASTER_SCL_IO           9
#define I2C_MASTER_SDA_IO           8
#define I2C_MASTER_FREQ_HZ          400000

#define BUTTON_GPIO                 0    // Boot button
#define LED_GPIO                    8    // Built-in LED (if available)

// Application Configuration
#define APP_VERSION                 "2.0.0"
#define APP_NAME                    "ESP32-C3 OLED Advanced"

#define DISPLAY_UPDATE_INTERVAL_MS  100
#define SENSOR_READ_INTERVAL_MS     1000
#define MENU_TIMEOUT_MS            10000

// WiFi Configuration
#define WIFI_SSID                   "YourWiFiSSID"
#define WIFI_PASS                   "YourWiFiPassword"
#define WIFI_MAXIMUM_RETRY          5

// Display Modes
typedef enum {
    DISPLAY_MODE_CLOCK = 0,
    DISPLAY_MODE_SYSTEM_INFO,
    DISPLAY_MODE_SENSOR_DATA,
    DISPLAY_MODE_NETWORK_INFO,
    DISPLAY_MODE_ANIMATIONS,
    DISPLAY_MODE_MENU,
    DISPLAY_MODE_MAX
} display_mode_t;

// Animation Types
typedef enum {
    ANIM_NONE = 0,
    ANIM_BOUNCING_BALL,
    ANIM_STARFIELD,
    ANIM_MATRIX_RAIN,
    ANIM_WAVE,
    ANIM_SPIRAL,
    ANIM_MAX
} animation_type_t;

#endif // APP_CONFIG_H
