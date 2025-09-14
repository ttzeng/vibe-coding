# API Reference

## Device Management

### Functions

#### `ssd1306_create()`
```c
ssd1306_handle_t ssd1306_create(i2c_master_bus_handle_t bus_handle, uint8_t dev_addr);
```
Creates a new SSD1306 device handle.

**Parameters:**
- `bus_handle`: I2C master bus handle
- `dev_addr`: I2C device address (typically 0x3C)

**Returns:** Device handle or NULL on error

#### `ssd1306_delete()`
```c
void ssd1306_delete(ssd1306_handle_t dev);
```
Deletes the SSD1306 device handle and frees resources.

#### `ssd1306_init()`
```c
esp_err_t ssd1306_init(ssd1306_handle_t dev);
```
Initializes the SSD1306 display with default settings.

### Display Control

#### `ssd1306_clear_screen()`
```c
void ssd1306_clear_screen(ssd1306_handle_t dev, uint8_t chFill);
```
Clears the display buffer.

**Parameters:**
- `chFill`: Fill pattern (0x00 for black, 0xFF for white)

#### `ssd1306_refresh_gram()`
```c
void ssd1306_refresh_gram(ssd1306_handle_t dev);
```
Updates the display with the current buffer contents.

### Graphics Functions

#### `ssd1306_draw_point()`
```c
void ssd1306_draw_point(ssd1306_handle_t dev, uint8_t chXpos, uint8_t chYpos, uint8_t chPoint);
```
Draws a single pixel.

**Parameters:**
- `chXpos`: X coordinate (0-127)
- `chYpos`: Y coordinate (0-63)
- `chPoint`: Color (0=black, 1=white)

#### `ssd1306_draw_line()`
```c
void ssd1306_draw_line(ssd1306_handle_t dev, uint8_t chXpos0, uint8_t chYpos0, 
                       uint8_t chXpos1, uint8_t chYpos1, uint8_t chMode);
```
Draws a line between two points.

#### `ssd1306_draw_rectangle()`
```c
void ssd1306_draw_rectangle(ssd1306_handle_t dev, uint8_t chXpos0, uint8_t chYpos0, 
                            uint8_t chWidth, uint8_t chHeight, uint8_t chMode);
```
Draws a rectangle outline.

### Text Functions

#### `ssd1306_show_char()`
```c
void ssd1306_show_char(ssd1306_handle_t dev, uint8_t chXpos, uint8_t chYpos, 
                       uint8_t chChr, uint8_t chSize, uint8_t chMode);
```
Displays a single character.

**Parameters:**
- `chChr`: ASCII character to display
- `chSize`: Font size (16 supported)
- `chMode`: Color (0=black, 1=white)

#### `ssd1306_show_string()`
```c
void ssd1306_show_string(ssd1306_handle_t dev, uint8_t chXpos, uint8_t chYpos, 
                         const char *pchString, uint8_t chSize, uint8_t chMode);
```
Displays a text string with automatic word wrapping.

## Display Manager

### Functions

#### `display_manager_create()`
```c
display_manager_handle_t display_manager_create(ssd1306_handle_t display);
```
Creates a new display manager instance.

**Parameters:**
- `display`: SSD1306 display handle

**Returns:** Display manager handle or NULL on error

#### `display_manager_set_mode()`
```c
esp_err_t display_manager_set_mode(display_manager_handle_t manager, display_mode_t mode);
```
Sets the current display mode.

**Parameters:**
- `manager`: Display manager handle
- `mode`: Display mode to set

**Returns:** ESP_OK on success

#### `display_manager_update()`
```c
esp_err_t display_manager_update(display_manager_handle_t manager);
```
Updates the display with current mode content.

## Animation System

### Functions

#### `animations_init()`
```c
esp_err_t animations_init(void);
```
Initializes the animation system.

#### `animations_set_type()`
```c
void animations_set_type(animation_type_t type);
```
Sets the current animation type.

#### `animations_update()`
```c
void animations_update(ssd1306_handle_t display, uint32_t frame);
```
Updates animation for current frame.

### Animation Types

```c
typedef enum {
    ANIM_NONE = 0,
    ANIM_BOUNCING_BALL,
    ANIM_STARFIELD, 
    ANIM_MATRIX_RAIN,
    ANIM_WAVE,
    ANIM_SPIRAL,
    ANIM_MAX
} animation_type_t;
```

## Sensor Manager

### Functions

#### `sensor_manager_init()`
```c
esp_err_t sensor_manager_init(void);
```
Initializes sensor hardware and data structures.

#### `sensor_manager_update()`
```c
esp_err_t sensor_manager_update(void);
```
Reads all sensors and updates cached data.

#### `sensor_manager_get_data()`
```c
sensor_data_t* sensor_manager_get_data(void);
```
Returns pointer to current sensor data.

### Data Structures

```c
typedef struct {
    float temperature;      // Temperature in Celsius
    float humidity;         // Relative humidity %
    float pressure;         // Atmospheric pressure hPa
    uint16_t light_level;   // Light level 0-1000
    bool data_valid;        // Data validity flag
    uint32_t last_update;   // Last update timestamp
} sensor_data_t;
```

## WiFi Manager

### Functions

#### `wifi_manager_init()`
```c
esp_err_t wifi_manager_init(void);
```
Initializes WiFi subsystem.

#### `wifi_manager_connect()`
```c
esp_err_t wifi_manager_connect(const char* ssid, const char* password);
```
Connects to specified WiFi network.

#### `wifi_manager_get_status()`
```c
wifi_status_t* wifi_manager_get_status(void);
```
Returns current WiFi status information.

### WiFi States

```c
typedef enum {
    WIFI_STATE_IDLE = 0,
    WIFI_STATE_CONNECTING,
    WIFI_STATE_CONNECTED,
    WIFI_STATE_DISCONNECTED,
    WIFI_STATE_ERROR
} wifi_state_t;
```

## Menu System

### Functions

#### `menu_system_init()`
```c
esp_err_t menu_system_init(void);
```
Initializes menu system.

#### `menu_system_navigate_up()`
```c
void menu_system_navigate_up(void);
```
Moves selection up in current menu.

#### `menu_system_select()`
```c
void menu_system_select(void);
```
Selects current menu item.

## Utility Functions

### Math Utilities

#### `utils_map_float()`
```c
float utils_map_float(float x, float in_min, float in_max, float out_min, float out_max);
```
Maps a float value from one range to another.

#### `utils_clamp_float()`
```c
float utils_clamp_float(float value, float min_val, float max_val);
```
Clamps a float value to specified range.

### Display Utilities

#### `utils_draw_progress_bar()`
```c
void utils_draw_progress_bar(void* display, int x, int y, int width, int height, 
                           float progress, bool border);
```
Draws a progress bar on the display.

#### `utils_draw_signal_strength()`
```c
void utils_draw_signal_strength(void* display, int x, int y, int8_t rssi);
```
Draws WiFi signal strength indicator.

## Error Codes

### Common Return Values
- `ESP_OK`: Operation successful
- `ESP_ERR_INVALID_ARG`: Invalid argument passed
- `ESP_ERR_INVALID_STATE`: Invalid state for operation
- `ESP_ERR_NO_MEM`: Out of memory
- `ESP_ERR_TIMEOUT`: Operation timed out
- `ESP_FAIL`: General failure

### Custom Error Codes
- `DISPLAY_ERR_NOT_INITIALIZED`: Display not initialized
- `SENSOR_ERR_READ_FAILED`: Sensor reading failed
- `WIFI_ERR_NOT_CONNECTED`: WiFi not connected
