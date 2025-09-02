# ESP32-C3 SSD1306 OLED Display Driver

A comprehensive ESP-IDF component for driving 0.96" SSD1306-based OLED displays using the ESP32-C3 Super Mini board with the modern I2C Master API.

## Features

- ✅ **Modern I2C Master API** - Uses the latest ESP-IDF I2C master driver
- ✅ **Component Architecture** - Clean separation as reusable ESP-IDF component
- ✅ **Graphics Primitives** - Points, lines, rectangles, and text rendering
- ✅ **Font Support** - Complete ASCII character set with 8x16 pixel font
- ✅ **Memory Efficient** - Optimized GRAM buffer management
- ✅ **Error Handling** - Comprehensive error checking and logging
- ✅ **Hardware Abstraction** - Easy to port to different GPIO configurations

## Hardware Requirements

### Components
- ESP32-C3 Super Mini development board
- 0.96" SSD1306 OLED display (128x64 pixels)
- Jumper wires

### Wiring Connections
| ESP32-C3 Super Mini | SSD1306 OLED |
|---------------------|--------------|
| 3.3V                | VCC          |
| GND                 | GND          |
| GPIO8               | SDA          |
| GPIO9               | SCL          |

## Software Requirements

- ESP-IDF v5.0 or later
- CMake 3.16 or later
- Python 3.6 or later

## Project Structure

```
esp32c3_oled_project/
├── README.md
├── CMakeLists.txt
├── main/
│   ├── CMakeLists.txt
│   └── main.c
└── components/
    └── ssd1306/
        ├── CMakeLists.txt
        ├── include/
        │   └── ssd1306.h
        └── ssd1306.c
```

## Quick Start

### 1. Clone or Create Project

Create a new directory and copy all the project files:

```bash
mkdir esp32c3_oled_project
cd esp32c3_oled_project
# Copy all project files here
```

### 2. Set Up ESP-IDF Environment

```bash
# Source ESP-IDF environment
# . $HOME/esp/esp-idf/export.sh
```

### 3. Configure Project

```bash
# Set target to ESP32-C3
idf.py set-target esp32c3

# Optional: Open menuconfig for additional settings
idf.py menuconfig
```

### 4. Build and Flash

```bash
# Build the project
idf.py build

# Flash to device (replace /dev/ttyUSB0 with your port)
idf.py -p /dev/ttyUSB0 flash

# Monitor serial output
idf.py -p /dev/ttyUSB0 monitor

# Or combine flash and monitor
idf.py -p /dev/ttyUSB0 flash monitor
```

## API Reference

### Device Management

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

## Configuration

### Display Settings

The following constants can be modified in `ssd1306.h`:

```c
#define SSD1306_I2C_ADDRESS     0x3C    // I2C address
#define SSD1306_WIDTH           128     // Display width
#define SSD1306_HEIGHT          64      // Display height
```

### GPIO Configuration

Modify the GPIO pins in `main.c`:

```c
#define I2C_MASTER_SCL_IO       9       // SCL pin
#define I2C_MASTER_SDA_IO       8       // SDA pin
#define I2C_MASTER_FREQ_HZ      400000  // I2C frequency
```

## Usage Example

```c
#include "ssd1306.h"

// Initialize I2C master bus
i2c_master_bus_handle_t bus_handle;
i2c_master_bus_config_t i2c_mst_config = {
    .clk_source = I2C_CLK_SRC_DEFAULT,
    .i2c_port = I2C_NUM_0,
    .scl_io_num = 9,
    .sda_io_num = 8,
    .glitch_ignore_cnt = 7,
    .flags.enable_internal_pullup = true,
};
i2c_new_master_bus(&i2c_mst_config, &bus_handle);

// Create and initialize display
ssd1306_handle_t display = ssd1306_create(bus_handle, SSD1306_I2C_ADDRESS);
ssd1306_init(display);

// Clear display
ssd1306_clear_screen(display, 0x00);

// Draw some graphics
ssd1306_show_string(display, 0, 0, "Hello World!", 16, 1);
ssd1306_draw_rectangle(display, 10, 20, 50, 30, 1);

// Update display
ssd1306_refresh_gram(display);
```

## Troubleshooting

### Common Issues

1. **Display not working**
   - Check wiring connections
   - Verify I2C address (use I2C scanner)
   - Ensure 3.3V power supply

2. **Compilation errors**
   - Verify ESP-IDF version (v5.0+)
   - Check component dependencies
   - Ensure proper project structure

3. **Garbled display**
   - Check I2C timing/frequency
   - Verify display initialization sequence
   - Ensure proper GRAM refresh calls

### Debug Tips

Enable debug logging in `menuconfig`:
```
Component config → Log output → Default log verbosity → Debug
```

Check I2C communication:
```bash
# Monitor I2C traffic
idf.py monitor
```

### I2C Scanner

Add this code to scan for I2C devices:

```c
void i2c_scanner(i2c_master_bus_handle_t bus_handle) {
    ESP_LOGI("I2C_SCANNER", "Scanning I2C bus...");
    for (uint8_t addr = 1; addr < 127; addr++) {
        i2c_device_config_t dev_cfg = {
            .dev_addr_length = I2C_ADDR_BIT_LEN_7,
            .device_address = addr,
            .scl_speed_hz = 100000,
        };
        
        i2c_master_dev_handle_t dev_handle;
        if (i2c_master_bus_add_device(bus_handle, &dev_cfg, &dev_handle) == ESP_OK) {
            uint8_t data = 0;
            if (i2c_master_transmit(dev_handle, &data, 1, 100) == ESP_OK) {
                ESP_LOGI("I2C_SCANNER", "Found device at address 0x%02X", addr);
            }
            i2c_master_bus_rm_device(dev_handle);
        }
    }
}
```

## Performance Notes

- **Refresh Rate**: ~60 FPS possible with full screen updates
- **Memory Usage**: 1024 bytes for GRAM buffer
- **I2C Speed**: 400kHz recommended for optimal performance
- **Power Consumption**: ~20mA typical operation

## Contributing

1. Fork the repository
2. Create a feature branch
3. Make your changes
4. Add tests if applicable
5. Submit a pull request

## License

This project is released under the MIT License. See LICENSE file for details.

## Acknowledgments

- ESP-IDF team for the excellent framework
- SSD1306 datasheet and reference implementations
- ESP32-C3 community for hardware insights

## Version History

- **v1.0.0** - Initial release with modern I2C Master API
  - Basic graphics primitives
  - Text rendering with 8x16 font
  - Full SSD1306 initialization sequence
  - Component-based architecture

---

**Note**: This README assumes you're familiar with ESP-IDF development. For beginners, please refer to the [ESP-IDF Getting Started Guide](https://docs.espressif.com/projects/esp-idf/en/latest/esp32c3/get-started/index.html).
