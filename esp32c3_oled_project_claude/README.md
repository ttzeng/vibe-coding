# ESP32-C3 Advanced OLED Display System

A comprehensive, production-ready ESP-IDF project featuring a multi-mode OLED display system with animations, WiFi management, sensor integration, and interactive menus for the ESP32-C3 Super Mini board.

![ESP32-C3 OLED Display](https://img.shields.io/badge/ESP32--C3-OLED%20Display-blue) ![ESP-IDF](https://img.shields.io/badge/ESP--IDF-v4.4%2B-green) ![License](https://img.shields.io/badge/License-MIT-yellow)

## 🌟 Features

### Display Modes
- 🕐 **Digital Clock** - Real-time display with NTP synchronization
- 💻 **System Monitor** - Heap usage, task count, uptime tracking
- 🌡️ **Sensor Dashboard** - Temperature, humidity, pressure readings
- 📶 **Network Status** - WiFi connection info, IP address, signal strength
- 🎨 **Animation Gallery** - 5+ smooth animations (bouncing ball, starfield, matrix rain, etc.)
- ⚙️ **Interactive Menu** - Button-controlled settings and configuration

### Advanced Features
- 🎮 **Button Navigation** - Single press to cycle modes, menu interaction
- 📡 **WiFi Management** - Auto-connect, scanning, status monitoring
- 💾 **Settings Storage** - NVS-based configuration persistence
- 🔧 **Component Architecture** - Modular, reusable ESP-IDF components
- 📊 **Real-time Monitoring** - Performance metrics, memory usage
- 🎯 **Error Handling** - Comprehensive error checking and recovery

## 📋 Table of Contents

- [Hardware Requirements](#hardware-requirements)
- [Software Requirements](#software-requirements)
- [Installation](#installation)
- [Wiring Guide](#wiring-guide)
- [Configuration](#configuration)
- [Usage](#usage)
- [Project Structure](#project-structure)
- [API Documentation](#api-documentation)
- [Customization](#customization)
- [Troubleshooting](#troubleshooting)
- [Contributing](#contributing)
- [License](#license)

## 🛠️ Hardware Requirements

### Essential Components
| Component | Specification | Notes |
|-----------|---------------|-------|
| **Microcontroller** | ESP32-C3 Super Mini | Main development board |
| **Display** | 0.96" SSD1306 OLED (128x64) | I2C interface |
| **Connections** | Jumper wires | For wiring connections |
| **Power** | USB-C cable | For programming and power |

### Optional Components
- Breadboard for prototyping
- External sensors (DHT22, BME280, etc.)
- Case/enclosure for finished project

## 💻 Software Requirements

- **ESP-IDF**: v4.4 or later
- **CMake**: v3.16 or later  
- **Python**: v3.6 or later
- **Git**: For cloning repository

### Platform Support
- ✅ Linux
- ✅ macOS
- ✅ Windows (with ESP-IDF tools)

## 🚀 Installation

### 1. Clone Repository
```bash
git clone https://github.com/your-username/esp32c3-oled-advanced.git
cd esp32c3-oled-advanced
```

### 2. Set Up ESP-IDF Environment
```bash
# Install ESP-IDF (if not already installed)
mkdir -p ~/esp
cd ~/esp
git clone --recursive https://github.com/espressif/esp-idf.git
cd esp-idf
./install.sh esp32c3

# Source the environment
. ~/esp/esp-idf/export.sh
# Or use: get_idf
```

### 3. Configure Project
```bash
# Set target
idf.py set-target esp32c3

# Configure project (optional)
idf.py menuconfig
```

### 4. Build and Flash
```bash
# Build project
idf.py build

# Flash to device (replace PORT with your serial port)
idf.py -p /dev/ttyUSB0 flash monitor

# Or combined command
idf.py -p /dev/ttyUSB0 flash monitor
```

## 🔌 Wiring Guide

### Pin Connections

| ESP32-C3 Super Mini | SSD1306 OLED | Function |
|---------------------|--------------|----------|
| **3.3V** | VCC | Power supply (3.3V) |
| **GND** | GND | Ground reference |
| **GPIO8** | SDA | I2C Data line |
| **GPIO9** | SCL | I2C Clock line |
| **GPIO0** | - | Boot button (built-in) |

### Wiring Diagram
```
ESP32-C3 Super Mini          SSD1306 OLED
    ┌─────────────┐            ┌──────────┐
    │         3V3 ├────────────┤ VCC      │
    │         GND ├────────────┤ GND      │
    │    GPIO8    ├────────────┤ SDA      │
    │    GPIO9    ├────────────┤ SCL      │
    │             │            │          │
    │    GPIO0    │ (Button)   └──────────┘
    └─────────────┘
```

### Important Notes
- ⚠️ **Voltage**: Ensure 3.3V power supply (not 5V)
- 🔧 **Pull-ups**: Internal I2C pull-ups are enabled in software
- 📍 **I2C Address**: Default is 0x3C (can be 0x3D on some displays)

## ⚙️ Configuration

### WiFi Setup
Edit `main/app_config.h`:
```c
#define WIFI_SSID                   "YourWiFiSSID"
#define WIFI_PASS                   "YourWiFiPassword"
```

### Display Settings
```c
#define I2C_MASTER_SCL_IO           9       // SCL pin
#define I2C_MASTER_SDA_IO           8       // SDA pin
#define I2C_MASTER_FREQ_HZ          400000  // I2C frequency
```

### Application Settings
```c
#define DISPLAY_UPDATE_INTERVAL_MS  100     // Display refresh rate
#define SENSOR_READ_INTERVAL_MS     1000    // Sensor update rate
#define MENU_TIMEOUT_MS            10000    // Menu auto-timeout
```

### Time Zone Configuration
Edit in `main/main.c`:
```c
// Set your local timezone
setenv("TZ", "EST5EDT,M3.2.0/2,M11.1.0", 1);  // Eastern Time
// setenv("TZ", "PST8PDT,M3.2.0,M11.1.0", 1);  // Pacific Time
// setenv("TZ", "CET-1CEST,M3.5.0,M10.5.0/3", 1); // Central European Time
```

## 🎮 Usage

### Display Mode Navigation
**Single Button Press**: Cycles through display modes
1. 🕐 **Clock Mode** → Shows current time, date, and uptime
2. 💻 **System Info** → Memory usage, task count, system stats
3. 🌡️ **Sensor Data** → Environmental readings and graphs
4. 📶 **Network Info** → WiFi status, IP address, signal strength
5. 🎨 **Animations** → Cycles through various animations
6. ⚙️ **Menu System** → Interactive configuration menu

### Menu System Navigation
When in **Menu Mode**:
- **Single Press**: Navigate down
- **Hold Press**: Select item
- **Auto-timeout**: Returns to clock mode after 10 seconds

### Menu Structure
```
📁 Main Menu
├── 🖥️ Display Settings
│   ├── Brightness +
│   ├── Brightness -
│   └── ← Back
├── 📡 Network Settings  
│   ├── WiFi Scan
│   ├── Disconnect WiFi
│   └── ← Back
├── ⚙️ System Settings
│   ├── System Info
│   ├── Factory Reset
│   ├── Reboot
│   └── ← Back
└── ℹ️ About
    ├── ESP32-C3 OLED
    ├── Version: 2.0.0
    ├── By: Your Name
    └── ← Back
```

### Animation Showcase
- **Bouncing Ball** - Physics simulation with collision detection
- **Starfield** - Scrolling stars with speed variations
- **Matrix Rain** - Digital rain effect with random characters
- **Wave Patterns** - Sine wave animations with particles
- **Spiral** - Rotating spiral with dynamic radius

## 📁 Project Structure

```
esp32c3_oled_advanced/
├── 📄 README.md                    # This file
├── 📄 CMakeLists.txt               # Root CMake configuration
├── 📄 sdkconfig.defaults           # Default ESP-IDF configuration
├── 📄 partitions.csv               # Partition table
├── 📁 main/                        # Main application
│   ├── 📄 main.c                   # Application entry point
│   ├── 📄 app_config.h             # Configuration constants
│   ├── 📄 display_manager.c/.h     # Display mode management
│   ├── 📄 menu_system.c/.h         # Interactive menu system
│   ├── 📄 sensor_manager.c/.h      # Sensor data handling
│   ├── 📄 wifi_manager.c/.h        # WiFi connection management
│   └── 📄 CMakeLists.txt           # Main CMake config
├── 📁 components/                  # Reusable components
│   ├── 📁 ssd1306/                 # OLED display driver
│   │   ├── 📄 ssd1306.c/.h         # Display driver implementation
│   │   └── 📄 CMakeLists.txt       # Component CMake config
│   ├── 📁 animations/              # Animation engine
│   │   ├── 📄 animations.c/.h      # Animation implementations
│   │   └── 📄 CMakeLists.txt       # Component CMake config
│   └── 📁 utils/                   # Utility functions
│       ├── 📄 utils.c/.h           # Helper functions
│       └── 📄 CMakeLists.txt       # Component CMake config
└── 📁 docs/                        # Documentation
    ├── 📄 API_Reference.md          # Detailed API documentation
    ├── 📄 Hardware_Setup.md         # Hardware setup guide
    └── 📄 User_Guide.md             # Comprehensive user guide
```

## 📚 API Documentation

### Core Components

#### Display Manager
```c
// Create display manager
display_manager_handle_t manager = display_manager_create(display_handle);

// Set display mode
display_manager_set_mode(manager, DISPLAY_MODE_CLOCK);

// Update display (call in main loop)
display_manager_update(manager);
```

#### Animation System
```c
// Initialize animations
animations_init();

// Set animation type
animations_set_type(ANIM_BOUNCING_BALL);

// Update animation frame
animations_update(display_handle, frame_number);
```

#### Sensor Manager
```c
// Initialize sensors
sensor_manager_init();

// Update sensor readings
sensor_manager_update();

// Get sensor data
sensor_data_t* data = sensor_manager_get_data();
printf("Temperature: %.1f°C\n", data->temperature);
```

### Display Modes
```c
typedef enum {
    DISPLAY_MODE_CLOCK = 0,
    DISPLAY_MODE_SYSTEM_INFO,
    DISPLAY_MODE_SENSOR_DATA,
    DISPLAY_MODE_NETWORK_INFO,
    DISPLAY_MODE_ANIMATIONS,
    DISPLAY_MODE_MENU,
    DISPLAY_MODE_MAX
} display_mode_t;
```

### Animation Types
```c
typedef enum {
    ANIM_BOUNCING_BALL,
    ANIM_STARFIELD,
    ANIM_MATRIX_RAIN,
    ANIM_WAVE,
    ANIM_SPIRAL,
    ANIM_MAX
} animation_type_t;
```

## 🎨 Customization

### Adding New Display Modes
1. **Define new mode** in `app_config.h`:
```c
typedef enum {
    DISPLAY_MODE_CLOCK = 0,
    // ... existing modes ...
    DISPLAY_MODE_YOUR_NEW_MODE,  // Add here
    DISPLAY_MODE_MAX
} display_mode_t;
```

2. **Implement display function** in `display_manager.c`:
```c
static void display_your_new_mode(display_manager_handle_t manager)
{
    ssd1306_show_string(manager->display, 0, 0, "Custom Mode", 16, 1);
    // Add your custom display logic here
}
```

3. **Add to switch statement**:
```c
switch (manager->current_mode) {
    // ... existing cases ...
    case DISPLAY_MODE_YOUR_NEW_MODE:
        display_your_new_mode(manager);
        break;
}
```

### Creating Custom Animations
1. **Add animation type**:
```c
typedef enum {
    // ... existing animations ...
    ANIM_YOUR_ANIMATION,
    ANIM_MAX
} animation_type_t;
```

2. **Implement animation function**:
```c
static void animate_your_animation(ssd1306_handle_t display, uint32_t frame)
{
    // Your animation logic here
    for (int x = 0; x < 128; x++) {
        int y = 32 + 20 * sin((x + frame * 2) * 0.1);
        ssd1306_draw_point(display, x, y, 1);
    }
}
```

### Adding Real Sensors
Replace simulated sensors in `sensor_manager.c`:
```c
esp_err_t sensor_read_temperature(float *temperature)
{
    // Replace simulation with actual sensor reading
    // Example for DHT22:
    // *temperature = dht22_read_temperature();
    return ESP_OK;
}
```

### Menu Customization
Add new menu items in `menu_system.c`:
```c
static const menu_item_t main_menu[] = {
    // ... existing items ...
    {"Your New Item", your_action_function, 0, false},
};
```

## 🔧 Troubleshooting

### Common Issues

#### Display Problems
| Issue | Possible Cause | Solution |
|-------|----------------|----------|
| Blank display | Wiring, power, or I2C address | Check connections, verify 3.3V power, try address 0x3D |
| Garbled text | Font corruption or I2C timing | Reduce I2C frequency, check font data integrity |
| Flickering | Insufficient power or EMI | Use quality USB cable, add decoupling capacitors |

#### WiFi Issues
| Issue | Possible Cause | Solution |
|-------|----------------|----------|
| Won't connect | Wrong credentials or weak signal | Verify SSID/password, check signal strength |
| Frequent disconnects | Power saving or interference | Disable power save mode, try different channel |
| Slow connection | Distance or interference | Move closer to router, check for interference |

#### System Issues
| Issue | Possible Cause | Solution |
|-------|----------------|----------|
| Boot loops | Memory issues or corrupted NVS | Erase flash, check heap usage |
| Button not working | Hardware or debouncing | Check GPIO0 connection, adjust debounce time |
| Crashes | Stack overflow or memory leaks | Increase stack sizes, monitor heap usage |

### Debug Commands
```bash
# Monitor serial output
idf.py monitor

# Erase flash completely
idf.py erase-flash

# Build with debug symbols
idf.py build -DCMAKE_BUILD_TYPE=Debug

# Monitor with custom baud rate
idf.py monitor -B 115200
```

### Enable Debug Logging
In `menuconfig`:
```
Component config → Log output → Default log verbosity → Debug
```

### Memory Analysis
```bash
# Check binary size
idf.py size

# Detailed size analysis
idf.py size-components

# Memory usage at runtime (add to code):
ESP_LOGI("HEAP", "Free: %d, Min: %d", 
         esp_get_free_heap_size(), 
         esp_get_minimum_free_heap_size());
```

## 🔍 Performance Optimization

### Memory Optimization
- Monitor heap usage with `esp_get_free_heap_size()`
- Use stack overflow detection in debug builds
- Optimize font data storage
- Consider PSRAM for large data structures

### Display Performance
- Minimize full screen redraws
- Use dirty region updates for animations
- Optimize I2C frequency (400kHz recommended)
- Batch I2C operations when possible

### Power Consumption
- Implement display sleep mode
- Use light sleep between updates
- Adjust sensor reading intervals
- Optimize WiFi usage patterns

## 🤝 Contributing

We welcome contributions! Please see our contributing guidelines:

### Getting Started
1. Fork the repository
2. Create a feature branch (`git checkout -b feature/amazing-feature`)
3. Make your changes
4. Add tests if applicable
5. Commit changes (`git commit -m 'Add amazing feature'`)
6. Push to branch (`git push origin feature/amazing-feature`)
7. Open a Pull Request

### Code Style
- Follow ESP-IDF coding standards
- Use meaningful variable and function names
- Add comprehensive comments
- Include error handling
- Update documentation for new features

### Testing
- Test on actual hardware
- Verify all display modes work
- Check memory usage
- Test WiFi connectivity
- Validate menu navigation

## 📄 License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

```
MIT License

Copyright (c) 2024 Your Name

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
```

## 🙏 Acknowledgments

- **ESP-IDF Team** - Excellent framework and documentation
- **SSD1306 Community** - Display driver implementations and examples
- **ESP32 Community** - Hardware insights and troubleshooting help
- **Open Source Contributors** - Various code snippets and inspiration

## 📞 Support

- 🐛 **Issues**: [GitHub Issues](https://github.com/your-username/esp32c3-oled-advanced/issues)
- 💬 **Discussions**: [GitHub Discussions](https://github.com/your-username/esp32c3-oled-advanced/discussions)
- 📧 **Email**: your-email@example.com
- 📖 **Documentation**: [Project Wiki](https://github.com/your-username/esp32c3-oled-advanced/wiki)

## 🔮 Future Enhancements

- [ ] **Touch Interface** - Capacitive touch sensor integration
- [ ] **Sound Effects** - Buzzer or speaker support
- [ ] **Data Logging** - SD card or flash storage
- [ ] **Web Interface** - Configuration via web browser
- [ ] **IoT Integration** - MQTT, HTTP APIs
- [ ] **More Sensors** - BME280, light sensors, motion detection
- [ ] **Power Management** - Battery operation, sleep modes
- [ ] **Custom Fonts** - Additional font sizes and styles

---

**⭐ If you find this project helpful, please give it a star! ⭐**

**🔧 Happy Making! 🔧**
