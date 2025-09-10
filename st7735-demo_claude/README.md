# ST7735 Display Driver for ESP32-C3 Super Mini

This ESP-IDF component provides a complete driver for the 1.8" ST7735-based TFT display with comprehensive graphics demonstration capabilities.

## Project Structure

```
st7735_demo/
├── README.md
├── CMakeLists.txt
├── main/
│   ├── CMakeLists.txt
│   └── main.c
└── components/
    └── st7735/
        ├── CMakeLists.txt
        ├── include/
        │   └── st7735.h
        ├── st7735_font.c
        ├── st7735_font.h
        └── st7735.c
```

## Hardware Connections (ESP32-C3 Super Mini)

| ST7735 Pin | ESP32-C3 Pin | Function |
|------------|--------------|----------|
| VCC        | 3.3V         | Power    |
| GND        | GND          | Ground   |
| SCL        | GPIO4        | SPI Clock |
| SDA        | GPIO6        | SPI MOSI  |
| RES        | GPIO0        | Reset     |
| DC         | GPIO1        | Data/Command |
| CS         | GPIO7        | Chip Select |
| BLK        | 3.3V         | Backlight (optional) |

## Features

### ST7735 Driver Features
- Full SPI master API integration with ESP-IDF
- Hardware reset support with proper timing
- Optimized initialization sequence for ST7735 displays
- 16-bit RGB565 color support
- 5×8 pixel bitmap font for all ASCII characters (32-127), including letters, numbers, symbols, and punctuation
- Bounds checking for all drawing operations
- DMA-enabled SPI transfers for optimal performance
- Memory-efficient streaming pixel operations

### Graphics Functions
- `st7735_fill_screen()` - Fill entire display with solid color
- `st7735_set_pixel()` - Set individual pixel with bounds checking
- `st7735_draw_hline()` - Draw horizontal line with clipping
- `st7735_draw_vline()` - Draw vertical line with clipping
- `st7735_draw_rect()` - Draw rectangle outline
- `st7735_fill_rect()` - Draw filled rectangle with clipping
- `st7735_draw_circle()` - Draw circle outline using Bresenham algorithm
- `st7735_fill_circle()` - Draw filled circle
- `st7735_rgb_to_565()` - RGB888 to RGB565 color conversion

### Text Drawing Functions:

- `st7735_draw_char()` - Draw individual characters with size scaling
- `st7735_draw_string()` - Draw complete strings with word wrap
- `st7735_printf()` - Printf-style formatted text output
- `st7735_set_cursor()` - Position text cursor
- `st7735_get_text_width()` - Calculate text width for centering
- `st7735_get_text_height()` - Get text height for layout

### Text Features:

- Variable Size: Text can be scaled 1x, 2x, 3x, etc.
- Color Control: Separate foreground and background colors
- Word Wrap: Automatic line wrapping at display edges
- Multi-line: Support for `\n` newline characters
- Printf Support: Format numbers, floats, strings like `printf()`

### Demo Showcase
- **Color Fills**: Full screen color transitions (Red, Green, Blue)
- **Rectangle Patterns**: Filled and outlined rectangles in various colors
- **Circle Patterns**: Concentric circles and filled circles
- **Line Patterns**: Horizontal and vertical line gradients
- **Animated Bouncing Ball**: Physics-based ball animation with wall collision
- **Gradient Effects**: Smooth color transitions across the display
- **Checkered Patterns**: Alternating tile patterns
- **Spiral Animations**: Mathematical spiral with rainbow colors
- **Status Displays**: System information screens
- **Sensor Dashboards**: Temperature, humidity, pressure readings
- **Digital Clocks**: Time and date display
- **Progress Indicators**: CPU, memory, network usage bars
- **Menu Systems**: Navigable option menus
- **Data Logging**: Formatted sensor readings

## Setup Instructions

### Prerequisites
- ESP-IDF v4.4 or later installed and configured
- ESP32-C3 Super Mini development board
- 1.8" ST7735-based TFT display
- Jumper wires for connections

### Installation Steps

1. **Create Project Directory**:
   ```bash
   mkdir st7735_demo
   cd st7735_demo
   ```

2. **Set up the file structure** as shown in the project structure above

3. **Copy all provided code files** into their respective directories

4. **Set ESP32-C3 as target**:
   ```bash
   idf.py set-target esp32c3
   ```

5. **Configure the project** (optional):
   ```bash
   idf.py menuconfig
   ```
   - Navigate to "Serial flasher config" → "Flash size" and ensure at least 2MB
   - Adjust partition table if needed

6. **Build and Flash**:
   ```bash
   idf.py build
   idf.py flash monitor
   ```

## Configuration Options

### SPI Configuration
- **Clock Speed**: 26 MHz (configurable in `st7735.c`)
- **SPI Mode**: Mode 0 (CPOL=0, CPHA=0)
- **DMA**: Enabled for transfers larger than 64 bytes
- **Data Width**: 8-bit command/data transfers
- **Host**: SPI2_HOST (HSPI)

### Display Settings
- **Resolution**: 128×160 pixels
- **Color Depth**: 16-bit (RGB565 format)
- **Orientation**: Portrait mode (configurable via MADCTL register)
- **Frame Rate**: ~60 FPS with optimized refresh
- **Color Space**: RGB565 (5 bits red, 6 bits green, 5 bits blue)

## API Reference

### Initialization
```c
esp_err_t st7735_init(st7735_handle_t *handle, const st7735_config_t *config);
esp_err_t st7735_deinit(st7735_handle_t *handle);
```

### Basic Drawing
```c
esp_err_t st7735_set_pixel(st7735_handle_t *handle, int x, int y, uint16_t color);
esp_err_t st7735_fill_screen(st7735_handle_t *handle, uint16_t color);
```

### Lines and Rectangles
```c
esp_err_t st7735_draw_hline(st7735_handle_t *handle, int x, int y, int w, uint16_t color);
esp_err_t st7735_draw_vline(st7735_handle_t *handle, int x, int y, int h, uint16_t color);
esp_err_t st7735_draw_rect(st7735_handle_t *handle, int x, int y, int w, int h, uint16_t color);
esp_err_t st7735_fill_rect(st7735_handle_t *handle, int x, int y, int w, int h, uint16_t color);
```

### Circles
```c
esp_err_t st7735_draw_circle(st7735_handle_t *handle, int x0, int y0, int r, uint16_t color);
esp_err_t st7735_fill_circle(st7735_handle_t *handle, int x0, int y0, int r, uint16_t color);
```

### Texts
```c
esp_err_t st7735_draw_char(st7735_handle_t *handle, int x, int y, char c, uint16_t color, uint16_t bg_color, uint8_t size);
esp_err_t st7735_draw_string(st7735_handle_t *handle, int x, int y, const char *str, uint16_t color, uint16_t bg_color, uint8_t size);
int st7735_get_text_width(const char *str, uint8_t size);
int st7735_get_text_height(uint8_t size);
esp_err_t st7735_set_cursor(st7735_handle_t *handle, int x, int y);
esp_err_t st7735_printf(st7735_handle_t *handle, uint16_t color, uint16_t bg_color, uint8_t size, const char *format, ...);

```

### Color Utilities
```c
uint16_t st7735_rgb_to_565(uint8_t r, uint8_t g, uint8_t b);
```

## Usage Example

### Basic Setup
```c
#include "st7735.h"

st7735_handle_t display;

// Configuration for ESP32-C3 Super Mini
st7735_config_t config = {
    .pin_mosi = 6,      // SDA
    .pin_sck = 4,       // SCL
    .pin_cs = 7,        // CS
    .pin_dc = 1,        // DC
    .pin_reset = 0,     // RES
    .spi_host = SPI2_HOST,
};

// Initialize display
esp_err_t ret = st7735_init(&display, &config);
if (ret != ESP_OK) {
    ESP_LOGE("MAIN", "Display initialization failed");
    return;
}
```

### Usage Examples
```c
// Clear screen
st7735_fill_screen(&display, ST7735_BLACK);

// Draw shapes
st7735_draw_circle(&display, 64, 80, 30, ST7735_RED);
st7735_fill_rect(&display, 10, 10, 50, 30, ST7735_BLUE);
st7735_draw_hline(&display, 0, 100, 128, ST7735_GREEN);

// Custom colors
uint16_t orange = st7735_rgb_to_565(255, 165, 0);
st7735_fill_circle(&display, 96, 40, 20, orange);

// Basic text
st7735_draw_string(&display, 10, 10, "Hello World!", ST7735_WHITE, ST7735_BLACK, 1);

// Formatted output
st7735_set_cursor(&display, 5, 5);
st7735_printf(&display, ST7735_GREEN, ST7735_BLACK, 2, "Temp: %.1fC", 23.5f);

// Large text
st7735_draw_string(&display, 5, 50, "BIG TEXT", ST7735_RED, ST7735_BLACK, 3);

// Menu systems
st7735_draw_string(&display, 10, 30, "Settings", ST7735_BLACK, ST7735_GREEN, 1); // Selected
st7735_draw_string(&display, 10, 45, "Display", ST7735_WHITE, ST7735_BLACK, 1);  // Normal
```

## Performance Optimization

### Drawing Performance
- **Bulk Operations**: Use `fill_rect()` instead of multiple `set_pixel()` calls
- **Color Caching**: Store frequently used colors in variables
- **Batch Updates**: Group drawing operations to minimize SPI overhead

### Memory Usage
- **Stack-Based**: All operations use minimal heap allocation
- **Streaming**: Large transfers stream data without buffering
- **Efficient Algorithms**: Optimized circle and line drawing algorithms

### SPI Optimization
```c
// In st7735.c, you can adjust clock speed:
.clock_speed_hz = 26 * 1000 * 1000,  // 26 MHz (increase if stable)
```

## Troubleshooting

### Display Issues

**No Display Output**:
1. Verify all wiring connections match the pin mapping
2. Check 3.3V power supply and ground connections
3. Ensure CS pin is connected (required for SPI communication)
4. Verify reset pin connection and timing

**Wrong Colors**:
1. Confirm RGB565 color format usage
2. Check byte order in SPI data transmission
3. Verify display variant (some ST7735 variants have different color mappings)

**Flickering or Corruption**:
1. Reduce SPI clock speed if experiencing instability
2. Check power supply stability (adequate current capacity)
3. Verify wiring integrity and minimize wire length
4. Add decoupling capacitors near the display

### Performance Issues

**Slow Drawing**:
1. Increase SPI clock speed (test stability first)
2. Use filled rectangles instead of individual pixels
3. Minimize frequent small operations
4. Enable SPI DMA for large transfers

**Memory Problems**:
1. Monitor stack usage in graphics-heavy functions
2. Use streaming operations for large data
3. Avoid large static buffers

## Advanced Features

### Custom Initialization
The driver allows customization of the ST7735 initialization sequence by modifying the `st7735_init()` function. Key registers:

- **MADCTL (0x36)**: Memory Access Control (rotation, mirroring)
- **COLMOD (0x3A)**: Color Mode (RGB565/RGB666)
- **Gamma Correction**: Fine-tune color reproduction

### Adding Text Support
To add text rendering capability:

1. Include a bitmap font array
2. Create character drawing functions
3. Implement string rendering with positioning

### Animation Framework
For smooth animations:

1. Use double buffering (if memory permits)
2. Implement frame rate limiting
3. Use hardware timers for consistent timing

## Color Reference

### Predefined Colors (RGB565)
```c
#define ST7735_BLACK   0x0000  // 0,   0,   0
#define ST7735_BLUE    0x001F  // 0,   0, 255
#define ST7735_RED     0xF800  // 255, 0,   0
#define ST7735_GREEN   0x07E0  // 0, 255,   0
#define ST7735_CYAN    0x07FF  // 0, 255, 255
#define ST7735_MAGENTA 0xF81F  // 255, 0, 255
#define ST7735_YELLOW  0xFFE0  // 255, 255, 0
#define ST7735_WHITE   0xFFFF  // 255, 255, 255
```

### Custom Color Creation
```c
// RGB888 to RGB565 conversion
uint16_t custom_color = st7735_rgb_to_565(128, 64, 192);  // Purple shade

// Direct RGB565 calculation
uint16_t direct_color = ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
```

## License

This ST7735 display driver component is provided under the MIT License for both educational and commercial use. Feel free to modify and distribute according to your project needs.

## Contributing

To contribute improvements:

1. Test thoroughly on ESP32-C3 hardware
2. Maintain compatibility with ESP-IDF framework
3. Follow ESP-IDF coding standards
4. Add appropriate error handling
5. Update documentation for new features

## Support

For issues and questions:

1. Check wiring against the pin mapping table
2. Verify ESP-IDF version compatibility
3. Test with the provided demo application
4. Review ESP-IDF SPI master documentation for advanced configuration
