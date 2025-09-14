#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

#include "esp_err.h"
#include "esp_wifi.h"

typedef enum {
    WIFI_STATE_IDLE = 0,
    WIFI_STATE_CONNECTING,
    WIFI_STATE_CONNECTED,
    WIFI_STATE_DISCONNECTED,
    WIFI_STATE_ERROR
} wifi_state_t;

typedef struct {
    char ssid[33];
    int8_t rssi;
    wifi_auth_mode_t authmode;
} wifi_ap_record_extended_t;

typedef struct {
    wifi_state_t state;
    char ssid[33];
    int8_t rssi;
    uint8_t ip_address[4];
    uint32_t connect_time;
    uint32_t reconnect_count;
    wifi_ap_record_extended_t scan_results[10];
    uint16_t scan_count;
} wifi_status_t;

// WiFi Manager API
esp_err_t wifi_manager_init(void);
esp_err_t wifi_manager_start(void);
esp_err_t wifi_manager_stop(void);
esp_err_t wifi_manager_connect(const char* ssid, const char* password);
esp_err_t wifi_manager_disconnect(void);
esp_err_t wifi_manager_scan(void);

wifi_status_t* wifi_manager_get_status(void);
bool wifi_manager_is_connected(void);

// Configuration
esp_err_t wifi_manager_save_config(const char* ssid, const char* password);
esp_err_t wifi_manager_load_config(void);

#endif // WIFI_MANAGER_H
