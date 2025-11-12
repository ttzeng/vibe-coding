# XPT2046 Touch Screen Driver for ESP32-C3

A comprehensive ESP-IDF driver for the XPT2046 resistive touch screen controller using the SPI Master API.

## Overview

This project provides a complete implementation for interfacing with XPT2046-based touch screens on ESP32-C3 microcontrollers. The driver uses ESP-IDF's SPI master driver and includes features like multi-sample averaging, calibration support, and both polling and interrupt-based touch detection.

## Features

- ✅ **ESP-IDF SPI Master API Integration**
- ✅ **Multi-sample Averaging** for improved accuracy
- ✅ **Calibration Support** with raw-to-screen coordinate mapping
- ✅ **IRQ Pin Support** for interrupt-based detection (optional)
- ✅ **Comprehensive Error Handling**
- ✅ **FreeRTOS Compatible**
- ✅ **Configurable Polling Rate**
- ✅ **Touch Pressure Detection**

## Hardware Requirements

- ESP32-C3 Super Mini Board
- XPT2046-based resistive touch screen
- 5 GPIO pins for SPI communication

## Pin Configuration

### Tested Configuration (ESP32)

| XPT2046 Pin | ESP32 GPIO | Description |
|-------------|------------|-------------|
| MISO (T_DO) | GPIO 5     | SPI Data Out |
| MOSI (T_DIN)| GPIO 6     | SPI Data In |
| CLK (T_CLK) | GPIO 4     | SPI Clock |
| CS (T_CS)   | GPIO 7     | Chip Select |
| IRQ (T_IRQ) | GPIO 3     | Interrupt (optional) |

> **Note**: Adjust pins according to your hardware setup in `main.c`

## Project Structure

```
xpt2046_touch/
├── CMakeLists.txt              # Root CMake configuration
├── README.md                   # This file
└── main/
    ├── CMakeLists.txt          # Component CMake configuration
    ├── xpt2046.h               # Driver header file
    ├── xpt2046.c               # Driver implementation
    └── main.c                  # Example application
```

## Getting Started

### Prerequisites

- ESP-IDF v4.4 or later
- Python 3.6 or later
- Configured ESP-IDF environment

### Installation

1. **Clone or create the project:**
```bash
mkdir xpt2046_touch
cd xpt2046_touch
```

2. **Copy all source files** to their respective directories as shown in the project structure.

3. **Configure the project (optional) :**
```bash
idf.py set-target esp32c3
idf.py menuconfig
```

4. **Update GPIO pins** in `main/main.c`:
```c
#define PIN_NUM_MISO    5
#define PIN_NUM_MOSI    6
#define PIN_NUM_CLK     4
#define PIN_NUM_CS      7
#define PIN_NUM_IRQ     3
```

### Building and Flashing

1. **Build the project:**
```bash
idf.py build
```

2. **Flash to ESP32:**
```bash
idf.py -p /dev/ttyUSB0 flash
```

3. **Monitor output:**
```bash
idf.py -p /dev/ttyUSB0 monitor
```

Or combine all steps:
```bash
idf.py -p /dev/ttyUSB0 flash monitor
```

## Usage

### Basic Example

```c
#include "xpt2046.h"

xpt2046_t touch_dev;
xpt2046_touch_t touch_data;

// Initialize
xpt2046_init(&touch_dev, 
             SPI2_HOST,
             PIN_NUM_MISO,
             PIN_NUM_MOSI,
             PIN_NUM_CLK,
             PIN_NUM_CS,
             PIN_NUM_IRQ);

// Set screen dimensions
touch_dev.screen_width = 320;
touch_dev.screen_height = 240;

// Read touch data
while (1) {
    xpt2046_read_touch(&touch_dev, &touch_data);
    
    if (touch_data.touched) {
        printf("X: %d, Y: %d\n", 
               touch_data.x_calibrated, 
               touch_data.y_calibrated);
    }
    
    vTaskDelay(pdMS_TO_TICKS(50));
}
```

### Calibration

The driver includes default calibration values, but for accurate touch detection, you should calibrate for your specific screen:

1. **Enable debug logging** in menuconfig:
   ```
   Component config → Log output → Default log verbosity → Debug
   ```

2. **Touch the corners** of your screen and note the raw values from the log output.

3. **Update calibration** in your code:
   ```c
   xpt2046_calibrate(&touch_dev, 
                     x_min,  // e.g., 200
                     x_max,  // e.g., 3900
                     y_min,  // e.g., 200
                     y_max); // e.g., 3900
   ```

### Optional: Calibration Mode

Add this function to `main.c` for easier calibration:

```c
void calibration_mode(xpt2046_t *dev)
{
    ESP_LOGI(TAG, "=== Calibration Mode ===");
    ESP_LOGI(TAG, "Touch each corner and note the values");
    
    for (int i = 0; i < 20; i++) {
        if (xpt2046_is_touched(dev)) {
            uint16_t x = xpt2046_read_channel(dev, XPT2046_DIFF_X);
            uint16_t y = xpt2046_read_channel(dev, XPT2046_DIFF_Y);
            ESP_LOGI(TAG, "Touch %d: X=%d, Y=%d", i, x, y);
        }
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}
```

## API Reference

### Initialization

```c
esp_err_t xpt2046_init(xpt2046_t *dev, 
                       spi_host_device_t spi_host,
                       gpio_num_t miso, 
                       gpio_num_t mosi,
                       gpio_num_t sclk, 
                       gpio_num_t cs, 
                       gpio_num_t irq);
```

### Reading Touch Data

```c
esp_err_t xpt2046_read_touch(xpt2046_t *dev, 
                              xpt2046_touch_t *touch_data);
```

Returns touch coordinates and pressure in `touch_data`:
- `x_raw`, `y_raw`: Raw ADC values (0-4095)
- `x_calibrated`, `y_calibrated`: Screen coordinates
- `z_raw`: Pressure value
- `touched`: Boolean indicating touch state

### Check Touch State

```c
bool xpt2046_is_touched(xpt2046_t *dev);
```

### Calibration

```c
void xpt2046_calibrate(xpt2046_t *dev, 
                       uint16_t x_min, 
                       uint16_t x_max,
                       uint16_t y_min, 
                       uint16_t y_max);
```

### Cleanup

```c
esp_err_t xpt2046_deinit(xpt2046_t *dev);
```

## Data Structures

### xpt2046_t (Device Handle)
```c
typedef struct {
    spi_device_handle_t spi;
    gpio_num_t cs_pin;
    gpio_num_t irq_pin;
    uint16_t x_min, x_max;
    uint16_t y_min, y_max;
    uint16_t screen_width;
    uint16_t screen_height;
} xpt2046_t;
```

### xpt2046_touch_t (Touch Data)
```c
typedef struct {
    uint16_t x_raw;
    uint16_t y_raw;
    uint16_t z_raw;
    uint16_t x_calibrated;
    uint16_t y_calibrated;
    bool touched;
} xpt2046_touch_t;
```

## Configuration Options

### SPI Configuration
- **Clock Speed**: 2 MHz (default, adjustable in `xpt2046.c`)
- **SPI Mode**: Mode 0 (CPOL=0, CPHA=0)
- **Bus**: SPI2_HOST (HSPI)

### Touch Sampling
- **Samples per read**: 8 (configurable in `xpt2046_read_touch()`)
- **Default polling rate**: 50ms (20 Hz)
- **Touch threshold**: 400 (adjustable in `xpt2046.h`)

## Troubleshooting

### No Touch Response

1. **Check wiring connections**
2. **Verify power supply** (3.3V for XPT2046)
3. **Enable debug logging**:
   ```bash
   idf.py menuconfig
   # Component config → Log output → Default log verbosity → Debug
   ```
4. **Test IRQ pin** (should be LOW when touched)

### Inaccurate Touch Coordinates

1. **Perform calibration** by touching screen corners
2. **Increase sample count** for more averaging
3. **Adjust touch threshold** in `xpt2046.h`
4. **Check for electromagnetic interference**

### SPI Communication Errors

1. **Verify GPIO pins** match your hardware
2. **Check SPI bus** isn't shared with conflicting devices
3. **Ensure proper ground connection**
4. **Try reducing SPI clock speed**

## Performance Considerations

- **Polling Rate**: 20 Hz (50ms delay) is recommended for responsive UI
- **Sample Count**: 8 samples provides good balance between accuracy and speed
- **CPU Usage**: Minimal (~1-2% at 20 Hz polling rate)
- **IRQ Mode**: Use IRQ pin for power-efficient applications

## Example Output

```
I (450) MAIN: Starting XPT2046 Touch Screen Example
I (460) MAIN: ESP-IDF Version: v5.1
I (470) XPT2046: XPT2046 initialized successfully
I (480) MAIN: Touch task started
I (1520) MAIN: === Touch Pressed ===
I (1520) MAIN: Position: X=156, Y=120 (Raw: 2048, 2100) Pressure: 850
I (1570) MAIN: Position: X=158, Y=121 (Raw: 2065, 2115) Pressure: 845
I (1620) MAIN: Position: X=157, Y=120 (Raw: 2055, 2108) Pressure: 848
I (1820) MAIN: === Touch Released ===
```

## Advanced Features

### Interrupt-Driven Touch Detection

For power-sensitive applications, implement GPIO ISR:

```c
static void IRAM_ATTR touch_isr_handler(void* arg)
{
    xTaskNotifyFromISR(touch_task_handle, 0, eNoAction, NULL);
}

// In initialization:
gpio_install_isr_service(0);
gpio_isr_handler_add(PIN_NUM_IRQ, touch_isr_handler, NULL);
gpio_set_intr_type(PIN_NUM_IRQ, GPIO_INTR_NEGEDGE);
```

### Integration with LVGL

The driver can be easily integrated with LVGL graphics library:

```c
void touchpad_read(lv_indev_drv_t *drv, lv_indev_data_t *data)
{
    xpt2046_touch_t touch_data;
    xpt2046_read_touch(&touch_dev, &touch_data);
    
    data->point.x = touch_data.x_calibrated;
    data->point.y = touch_data.y_calibrated;
    data->state = touch_data.touched ? LV_INDEV_STATE_PR : LV_INDEV_STATE_REL;
}
```

## Contributing

Contributions are welcome! Please feel free to submit issues or pull requests.

## License

This project is provided as-is for educational and commercial use.

## References

- [XPT2046 Datasheet](https://grobotronics.com/images/datasheets/xpt2046-datasheet.pdf?srsltid=AfmBOorhjugWjsrw06cAMOTdPmnNR7QVf3ysAlbqEIWLGugHE4E-sCqM)
- [ESP-IDF SPI Master Documentation](https://docs.espressif.com/projects/esp-idf/en/latest/esp32c3/api-reference/peripherals/spi_master.html)
- [ESP-IDF Programming Guide](https://docs.espressif.com/projects/esp-idf/en/latest/esp32c3/get-started/index.html)
---

**Happy Coding! 🚀**
