#include <string.h>
#include "wifi_manager.h"
#include "esp_log.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_netif.h"
#include "nvs_flash.h"
#include "nvs.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"

static const char *TAG = "WIFI_MGR";

// WiFi event bits
#define WIFI_CONNECTED_BIT BIT0
#define WIFI_FAIL_BIT      BIT1

static EventGroupHandle_t s_wifi_event_group;
static wifi_status_t g_wifi_status = {0};
static bool initialized = false;
static int s_retry_num = 0;

// Forward declarations
static void wifi_event_handler(void* arg, esp_event_base_t event_base,
                              int32_t event_id, void* event_data);

esp_err_t wifi_manager_init(void)
{
    if (initialized) {
        return ESP_OK;
    }
    
    esp_err_t ret = ESP_OK;
    
    // Initialize networking stack
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_create_default_wifi_sta();
    
    // Initialize WiFi
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    
    // Create event group
    s_wifi_event_group = xEventGroupCreate();
    
    // Register event handlers
    ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT,
                                             ESP_EVENT_ANY_ID,
                                             &wifi_event_handler,
                                             NULL));
    ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT,
                                             IP_EVENT_STA_GOT_IP,
                                             &wifi_event_handler,
                                             NULL));
    
    // Initialize status
    g_wifi_status.state = WIFI_STATE_IDLE;
    g_wifi_status.rssi = 0;
    g_wifi_status.connect_time = 0;
    g_wifi_status.reconnect_count = 0;
    g_wifi_status.scan_count = 0;
    memset(g_wifi_status.ssid, 0, sizeof(g_wifi_status.ssid));
    memset(g_wifi_status.ip_address, 0, sizeof(g_wifi_status.ip_address));
    
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    
    initialized = true;
    ESP_LOGI(TAG, "WiFi manager initialized");
    
    // Try to load saved configuration and connect
    wifi_manager_load_config();
    
    return ret;
}

esp_err_t wifi_manager_start(void)
{
    if (!initialized) {
        return ESP_ERR_INVALID_STATE;
    }
    
    return esp_wifi_start();
}

esp_err_t wifi_manager_stop(void)
{
    if (!initialized) {
        return ESP_ERR_INVALID_STATE;
    }
    
    g_wifi_status.state = WIFI_STATE_IDLE;
    return esp_wifi_stop();
}

esp_err_t wifi_manager_connect(const char* ssid, const char* password)
{
    if (!initialized || ssid == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    
    wifi_config_t wifi_config = {0};
    strncpy((char*)wifi_config.sta.ssid, ssid, sizeof(wifi_config.sta.ssid) - 1);
    if (password) {
        strncpy((char*)wifi_config.sta.password, password, sizeof(wifi_config.sta.password) - 1);
    }
    
    wifi_config.sta.threshold.authmode = WIFI_AUTH_WPA2_PSK;
    wifi_config.sta.pmf_cfg.capable = true;
    wifi_config.sta.pmf_cfg.required = false;
    
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());
    
    g_wifi_status.state = WIFI_STATE_CONNECTING;
    strncpy(g_wifi_status.ssid, ssid, sizeof(g_wifi_status.ssid) - 1);
    s_retry_num = 0;
    
    ESP_LOGI(TAG, "Connecting to WiFi SSID: %s", ssid);
    
    // Wait for connection result
    EventBits_t bits = xEventGroupWaitBits(s_wifi_event_group,
                                          WIFI_CONNECTED_BIT | WIFI_FAIL_BIT,
                                          pdFALSE,
                                          pdFALSE,
                                          portMAX_DELAY);
    
    if (bits & WIFI_CONNECTED_BIT) {
        ESP_LOGI(TAG, "Connected to WiFi");
        return ESP_OK;
    } else if (bits & WIFI_FAIL_BIT) {
        ESP_LOGI(TAG, "Failed to connect to WiFi");
        return ESP_FAIL;
    }
    
    return ESP_ERR_TIMEOUT;
}

esp_err_t wifi_manager_disconnect(void)
{
    if (!initialized) {
        return ESP_ERR_INVALID_STATE;
    }
    
    g_wifi_status.state = WIFI_STATE_DISCONNECTED;
    return esp_wifi_disconnect();
}

esp_err_t wifi_manager_scan(void)
{
    if (!initialized) {
        return ESP_ERR_INVALID_STATE;
    }
    
    ESP_LOGI(TAG, "Starting WiFi scan");
    
    wifi_scan_config_t scan_config = {0};
    return esp_wifi_scan_start(&scan_config, false);
}

wifi_status_t* wifi_manager_get_status(void)
{
    return &g_wifi_status;
}

bool wifi_manager_is_connected(void)
{
    return (g_wifi_status.state == WIFI_STATE_CONNECTED);
}

esp_err_t wifi_manager_save_config(const char* ssid, const char* password)
{
    if (!ssid) {
        return ESP_ERR_INVALID_ARG;
    }
    
    nvs_handle_t nvs_handle;
    esp_err_t err = nvs_open("wifi_config", NVS_READWRITE, &nvs_handle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Error opening NVS handle: %s", esp_err_to_name(err));
        return err;
    }
    
    // Save SSID
    err = nvs_set_str(nvs_handle, "ssid", ssid);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Error saving SSID: %s", esp_err_to_name(err));
        nvs_close(nvs_handle);
        return err;
    }
    
    // Save password (if provided)
    if (password) {
        err = nvs_set_str(nvs_handle, "password", password);
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "Error saving password: %s", esp_err_to_name(err));
            nvs_close(nvs_handle);
            return err;
        }
    }
    
    err = nvs_commit(nvs_handle);
    nvs_close(nvs_handle);
    
    ESP_LOGI(TAG, "WiFi configuration saved");
    return err;
}

esp_err_t wifi_manager_load_config(void)
{
    nvs_handle_t nvs_handle;
    esp_err_t err = nvs_open("wifi_config", NVS_READONLY, &nvs_handle);
    if (err != ESP_OK) {
        ESP_LOGW(TAG, "No saved WiFi configuration found");
        return err;
    }
    
    char ssid[33] = {0};
    char password[65] = {0};
    size_t ssid_len = sizeof(ssid);
    size_t pass_len = sizeof(password);
    
    // Load SSID
    err = nvs_get_str(nvs_handle, "ssid", ssid, &ssid_len);
    if (err != ESP_OK) {
        ESP_LOGW(TAG, "Error loading SSID: %s", esp_err_to_name(err));
        nvs_close(nvs_handle);
        return err;
    }
    
    // Load password
    err = nvs_get_str(nvs_handle, "password", password, &pass_len);
    if (err != ESP_OK && err != ESP_ERR_NVS_NOT_FOUND) {
        ESP_LOGW(TAG, "Error loading password: %s", esp_err_to_name(err));
    }
    
    nvs_close(nvs_handle);
    
    ESP_LOGI(TAG, "Loaded WiFi configuration: %s", ssid);
    
    // Attempt to connect
    return wifi_manager_connect(ssid, password);
}

static void wifi_event_handler(void* arg, esp_event_base_t event_base,
                              int32_t event_id, void* event_data)
{
    if (event_base == WIFI_EVENT) {
        switch (event_id) {
            case WIFI_EVENT_STA_START:
                ESP_LOGI(TAG, "WiFi station started");
                esp_wifi_connect();
                break;
                
            case WIFI_EVENT_STA_DISCONNECTED:
                {
                    wifi_event_sta_disconnected_t* disconnected = (wifi_event_sta_disconnected_t*) event_data;
                    ESP_LOGI(TAG, "WiFi disconnected, reason: %d", disconnected->reason);
                    
                    g_wifi_status.state = WIFI_STATE_DISCONNECTED;
                    g_wifi_status.rssi = 0;
                    memset(g_wifi_status.ip_address, 0, sizeof(g_wifi_status.ip_address));
                    
                    if (s_retry_num < 5) {
                        esp_wifi_connect();
                        s_retry_num++;
                        g_wifi_status.reconnect_count++;
                        ESP_LOGI(TAG, "Retry to connect to the AP");
                    } else {
                        xEventGroupSetBits(s_wifi_event_group, WIFI_FAIL_BIT);
                        g_wifi_status.state = WIFI_STATE_ERROR;
                    }
                }
                break;
                
            case WIFI_EVENT_SCAN_DONE:
                {
                    wifi_event_sta_scan_done_t* scan_done = (wifi_event_sta_scan_done_t*) event_data;
                    ESP_LOGI(TAG, "WiFi scan completed, found %d APs", scan_done->number);
                    
                    uint16_t ap_count = 0;
                    esp_wifi_scan_get_ap_num(&ap_count);
                    
                    if (ap_count > 0) {
                        wifi_ap_record_t* ap_records = malloc(sizeof(wifi_ap_record_t) * ap_count);
                        if (ap_records) {
                            esp_wifi_scan_get_ap_records(&ap_count, ap_records);
                            
                            // Copy to our extended format (limited to 10 entries)
                            g_wifi_status.scan_count = (ap_count > 10) ? 10 : ap_count;
                            for (int i = 0; i < g_wifi_status.scan_count; i++) {
                                strncpy(g_wifi_status.scan_results[i].ssid, 
                                       (char*)ap_records[i].ssid, 32);
                                g_wifi_status.scan_results[i].rssi = ap_records[i].rssi;
                                g_wifi_status.scan_results[i].authmode = ap_records[i].authmode;
                            }
                            
                            free(ap_records);
                        }
                    }
                }
                break;
                
            default:
                break;
        }
    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;
        ESP_LOGI(TAG, "Got IP address: " IPSTR, IP2STR(&event->ip_info.ip));
        
        g_wifi_status.state = WIFI_STATE_CONNECTED;
        g_wifi_status.connect_time = xTaskGetTickCount() * portTICK_PERIOD_MS;
        
        // Store IP address
        g_wifi_status.ip_address[0] = esp_ip4_addr1_16(&event->ip_info.ip);
        g_wifi_status.ip_address[1] = esp_ip4_addr2_16(&event->ip_info.ip);
        g_wifi_status.ip_address[2] = esp_ip4_addr3_16(&event->ip_info.ip);
        g_wifi_status.ip_address[3] = esp_ip4_addr4_16(&event->ip_info.ip);
        
        // Get signal strength
        wifi_ap_record_t ap_info;
        if (esp_wifi_sta_get_ap_info(&ap_info) == ESP_OK) {
            g_wifi_status.rssi = ap_info.rssi;
        }
        
        s_retry_num = 0;
        xEventGroupSetBits(s_wifi_event_group, WIFI_CONNECTED_BIT);
    }
}
