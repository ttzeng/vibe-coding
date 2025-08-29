# ADXL345 ESP32-C3 ESP-IDF Driver Documentation

## Overview

*This project is [created][1] by Claude Sonnet 3.7*

This document provides comprehensive documentation for the ADXL345 3-axis accelerometer driver designed for ESP32-C3 microcontroller using the ESP-IDF framework. The driver utilizes the new I2C master API introduced in ESP-IDF v5.0+ for improved performance and cleaner code structure.

## Table of Contents

- [Features](#features)
- [Hardware Requirements](#hardware-requirements)
- [Hardware Wiring](#hardware-wiring)
- [Software Requirements](#software-requirements)
- [Project Structure](#project-structure)
- [Installation](#installation)
- [API Reference](#api-reference)
- [Configuration](#configuration)
- [Usage Examples](#usage-examples)
- [Troubleshooting](#troubleshooting)
- [License](#license)

## Features

### Driver Features
- ✅ **New I2C Master API**: Uses ESP-IDF v5.0+ I2C master API for better performance
- ✅ **Handle-based Architecture**: Multiple ADXL345 instances support
- ✅ **Complete Register Support**: Full ADXL345 register map implementation
- ✅ **Multiple Measurement Ranges**: ±2g, ±4g, ±8g, ±16g
- ✅ **Configurable Data Rates**: 25Hz to 3200Hz
- ✅ **Auto-scaling**: Raw data automatically converted to g-force units
- ✅ **Error Handling**: Comprehensive ESP-IDF error code support
- ✅ **Memory Management**: Automatic allocation and cleanup
- ✅ **Thread Safe**: Multiple instances can coexist safely

### Demo Application Features
- 🔄 **Real-time Data Reading**: Continuous acceleration monitoring
- 📊 **Raw and Scaled Data**: Both raw ADC values and g-force units
- 🎯 **Motion Detection**: Simple activity detection algorithm
- 📐 **Tilt Calculation**: X and Y axis tilt angle computation
- 📈 **Magnitude Calculation**: 3D acceleration vector magnitude
- 🔍 **Device Verification**: Automatic device ID checking

## Hardware Requirements

### Microcontroller
- **ESP32-C3** (ESP32-C3-DevKitM-1 or similar)
- **ESP-IDF v5.0+** framework support

### Accelerometer
- **ADXL345** 3-axis digital accelerometer
- I2C interface (address: 0x53)
- 3.3V or 5V compatible (3.3V recommended)

### Additional Components
- 2x **Pull-up resistors** (4.7kΩ recommended) for SDA/SCL lines (if not using internal pull-ups)
- **Breadboard** and **jumper wires**
- **Power supply** (3.3V)

## Hardware Wiring

### Wiring Diagram

```
ESP32-C3                    ADXL345
┌─────────────┐            ┌─────────────┐
│             │            │             │
│         GND ├────────────┤ GND         │
│        3.3V ├────────────┤ VCC         │
│             │            │             │
│    GPIO8    ├────────────┤ SDA         │
│    (SDA)    │    4.7kΩ   │             │
│             │     ┃      │             │
│    GPIO9    ├─────╂──────┤ SCL         │
│    (SCL)    │     ┃      │             │
│             │     ┃      │             │
│        3.3V ├─────┻──────┤ (Pull-ups)  │
│             │            │             │
└─────────────┘            └─────────────┘
```

### Pin Connections

| ESP32-C3 Pin | ADXL345 Pin | Description | Notes |
|-------------|-------------|-------------|--------|
| GND | GND | Ground | Common ground connection |
| 3.3V | VCC | Power Supply | 3.3V recommended |
| GPIO8 | SDA | I2C Data Line | Requires pull-up resistor |
| GPIO9 | SCL | I2C Clock Line | Requires pull-up resistor |

### Pull-up Resistors

You can use either:
1. **External pull-up resistors**: 4.7kΩ resistors from SDA/SCL to 3.3V
2. **Internal pull-ups**: Set `enable_pullup = true` in driver configuration

## Software Requirements

### ESP-IDF Version
- **Minimum**: ESP-IDF v5.0.0
- **Recommended**: ESP-IDF v5.1.0 or later
- **Components**: `driver` (for I2C master API)

### Build System
- **CMake** build system
- **Component-based** architecture

## Project Structure

```
adxl345_demo/
├── CMakeLists.txt                 # Root build configuration
├── README.md                      # This documentation
├── main/
│   ├── CMakeLists.txt            # Main component build config
│   └── main.c                    # Demo application
└── components/
    └── adxl345/
        ├── CMakeLists.txt        # Component build configuration
        ├── include/
        │   └── adxl345.h         # Driver header file
        └── adxl345.c             # Driver implementation
```

## Installation

### 1. Create Project Structure

```bash
mkdir adxl345_demo
cd adxl345_demo
mkdir -p main components/adxl345/include
```

### 2. Copy Files

Copy all the provided source files into their respective directories according to the project structure above.

### 3. Build and Flash

```bash
# Set ESP-IDF environment
. $HOME/esp/esp-idf/export.sh

# Configure project for ESP32-C3
idf.py set-target esp32c3

# Build project
idf.py build

# Flash and monitor
idf.py flash monitor
```

## API Reference

### Data Structures

#### `adxl345_config_t`
Configuration structure for initializing the ADXL345 driver.

```c
typedef struct {
    gpio_num_t sda_pin;        // I2C SDA pin
    gpio_num_t scl_pin;        // I2C SCL pin  
    uint32_t clk_speed;        // I2C clock speed in Hz
    bool enable_pullup;        // Enable internal pull-up resistors
} adxl345_config_t;
```

#### `adxl345_handle_t`
Opaque handle structure for ADXL345 device instance.

#### `adxl345_accel_data_t`
Structure containing acceleration data in g-force units.

```c
typedef struct {
    float x;  // X-axis acceleration in g
    float y;  // Y-axis acceleration in g
    float z;  // Z-axis acceleration in g
} adxl345_accel_data_t;
```

#### `adxl345_range_t`
Enumeration for measurement range settings.

```c
typedef enum {
    ADXL345_RANGE_2G = 0,   // ±2g range
    ADXL345_RANGE_4G,       // ±4g range
    ADXL345_RANGE_8G,       // ±8g range
    ADXL345_RANGE_16G       // ±16g range
} adxl345_range_t;
```

### Core Functions

#### `adxl345_init()`
Initialize the ADXL345 driver and create device handle.

```c
esp_err_t adxl345_init(const adxl345_config_t *config, adxl345_handle_t **handle);
```

**Parameters:**
- `config`: Pointer to configuration structure
- `handle`: Pointer to handle pointer (output parameter)

**Returns:** `ESP_OK` on success, error code otherwise

#### `adxl345_deinit()`
Deinitialize the driver and free resources.

```c
esp_err_t adxl345_deinit(adxl345_handle_t *handle);
```

**Parameters:**
- `handle`: Device handle

**Returns:** `ESP_OK` on success, error code otherwise

#### `adxl345_read_acceleration()`
Read acceleration data in g-force units.

```c
esp_err_t adxl345_read_acceleration(adxl345_handle_t *handle, adxl345_accel_data_t *data);
```

**Parameters:**
- `handle`: Device handle
- `data`: Pointer to acceleration data structure

**Returns:** `ESP_OK` on success, error code otherwise

### Configuration Functions

#### `adxl345_set_range()`
Set the measurement range.

```c
esp_err_t adxl345_set_range(adxl345_handle_t *handle, adxl345_range_t range);
```

#### `adxl345_set_data_rate()`
Set the data output rate.

```c
esp_err_t adxl345_set_data_rate(adxl345_handle_t *handle, uint8_t rate);
```

**Rate Constants:**
- `ADXL345_BW_RATE_25HZ` to `ADXL345_BW_RATE_3200HZ`

#### `adxl345_enable_measurement()`
Enable measurement mode.

```c
esp_err_t adxl345_enable_measurement(adxl345_handle_t *handle);
```

#### `adxl345_disable_measurement()`
Disable measurement mode (standby).

```c
esp_err_t adxl345_disable_measurement(adxl345_handle_t *handle);
```

### Utility Functions

#### `adxl345_check_device_id()`
Verify device ID (should return 0xE5).

```c
esp_err_t adxl345_check_device_id(adxl345_handle_t *handle);
```

#### `adxl345_read_raw_data()`
Read raw ADC values.

```c
esp_err_t adxl345_read_raw_data(adxl345_handle_t *handle, int16_t *x, int16_t *y, int16_t *z);
```

## Configuration

### Basic Configuration Example

```c
adxl345_config_t config = {
    .sda_pin = GPIO_NUM_8,
    .scl_pin = GPIO_NUM_9,
    .clk_speed = 400000,  // 400kHz
    .enable_pullup = true // Use internal pull-ups
};
```

### Advanced Configuration

```c
// High-speed configuration
adxl345_config_t high_speed_config = {
    .sda_pin = GPIO_NUM_8,
    .scl_pin = GPIO_NUM_9,
    .clk_speed = 1000000,  // 1MHz
    .enable_pullup = false // Use external pull-ups
};

// Low-power configuration  
adxl345_config_t low_power_config = {
    .sda_pin = GPIO_NUM_8,
    .scl_pin = GPIO_NUM_9,
    .clk_speed = 100000,   // 100kHz
    .enable_pullup = true
};
```

## Usage Examples

### Basic Usage

```c
#include "adxl345.h"

void app_main(void)
{
    // Configure driver
    adxl345_config_t config = {
        .sda_pin = GPIO_NUM_8,
        .scl_pin = GPIO_NUM_9,
        .clk_speed = 400000,
        .enable_pullup = true
    };

    // Initialize driver
    adxl345_handle_t *handle;
    esp_err_t ret = adxl345_init(&config, &handle);
    if (ret != ESP_OK) {
        printf("Failed to initialize ADXL345\n");
        return;
    }

    // Read acceleration data
    adxl345_accel_data_t accel;
    while (1) {
        ret = adxl345_read_acceleration(handle, &accel);
        if (ret == ESP_OK) {
            printf("X: %.3f g, Y: %.3f g, Z: %.3f g\n", 
                   accel.x, accel.y, accel.z);
        }
        vTaskDelay(pdMS_TO_TICKS(100));
    }

    // Cleanup
    adxl345_deinit(handle);
}
```

### Motion Detection Example

```c
void motion_detection_task(void *arg)
{
    adxl345_handle_t *handle = (adxl345_handle_t *)arg;
    adxl345_accel_data_t accel;
    
    while (1) {
        if (adxl345_read_acceleration(handle, &accel) == ESP_OK) {
            float magnitude = sqrtf(accel.x * accel.x + 
                                   accel.y * accel.y + 
                                   accel.z * accel.z);
            
            if (magnitude > 1.5f) {  // Threshold for motion
                printf("Motion detected! Magnitude: %.3f g\n", magnitude);
            }
        }
        vTaskDelay(pdMS_TO_TICKS(50));
    }
}
```

### Tilt Measurement Example

```c
void tilt_measurement(adxl345_handle_t *handle)
{
    adxl345_accel_data_t accel;
    
    if (adxl345_read_acceleration(handle, &accel) == ESP_OK) {
        // Calculate tilt angles (assuming device is relatively static)
        float pitch = asinf(-accel.x) * 180.0f / M_PI;  // Rotation around Y-axis
        float roll = asinf(accel.y) * 180.0f / M_PI;    // Rotation around X-axis
        
        printf("Pitch: %.1f°, Roll: %.1f°\n", pitch, roll);
    }
}
```

### Multiple Device Example

```c
void multiple_sensors_example(void)
{
    // Configure first ADXL345 (assuming different I2C address via SDO pin)
    adxl345_config_t config1 = {
        .sda_pin = GPIO_NUM_8,
        .scl_pin = GPIO_NUM_9,
        .clk_speed = 400000,
        .enable_pullup = true
    };

    // Configure second ADXL345 on different pins
    adxl345_config_t config2 = {
        .sda_pin = GPIO_NUM_18,
        .scl_pin = GPIO_NUM_19,
        .clk_speed = 400000,
        .enable_pullup = true
    };

    adxl345_handle_t *handle1, *handle2;
    
    // Initialize both devices
    adxl345_init(&config1, &handle1);
    adxl345_init(&config2, &handle2);

    // Use both devices independently
    adxl345_accel_data_t accel1, accel2;
    
    while (1) {
        adxl345_read_acceleration(handle1, &accel1);
        adxl345_read_acceleration(handle2, &accel2);
        
        printf("Sensor 1: X=%.3f, Y=%.3f, Z=%.3f\n", 
               accel1.x, accel1.y, accel1.z);
        printf("Sensor 2: X=%.3f, Y=%.3f, Z=%.3f\n", 
               accel2.x, accel2.y, accel2.z);
               
        vTaskDelay(pdMS_TO_TICKS(200));
    }
}
```

## Troubleshooting

### Common Issues

#### 1. Device ID Check Failed
**Symptoms:** `ESP_ERR_INVALID_RESPONSE` during initialization

**Possible Causes:**
- Incorrect wiring connections
- Wrong I2C address
- Power supply issues
- Missing pull-up resistors

**Solutions:**
```c
// Check wiring connections
// Verify 3.3V power supply
// Add 4.7kΩ pull-up resistors on SDA/SCL
// Use multimeter to verify continuity
```

#### 2. I2C Communication Timeout
**Symptoms:** `ESP_ERR_TIMEOUT` errors

**Possible Causes:**
- Clock speed too high
- Interference on I2C lines
- Long wire connections

**Solutions:**
```c
// Reduce I2C clock speed
adxl345_config_t config = {
    .clk_speed = 100000,  // Reduce to 100kHz
    // ... other settings
};

// Add delay between operations
vTaskDelay(pdMS_TO_TICKS(10));
```

#### 3. Incorrect Readings
**Symptoms:** Constant zero values or unrealistic readings

**Possible Causes:**
- Device not in measurement mode
- Wrong range setting
- Faulty sensor

**Solutions:**
```c
// Ensure measurement is enabled
adxl345_enable_measurement(handle);

// Check device status
uint8_t power_ctl;
adxl345_read_reg(handle, ADXL345_REG_POWER_CTL, &power_ctl);
printf("Power control register: 0x%02X\n", power_ctl);
```

#### 4. Build Errors
**Symptoms:** Compilation or linking errors

**Solutions:**
```bash
# Ensure ESP-IDF v5.0+
idf.py --version

# Clean and rebuild
idf.py clean
idf.py build

# Check CMakeLists.txt files are correct
```

### Debug Tips

#### Enable Debug Logging
```c
// In main.c, add before esp_log.h include:
#define LOG_LOCAL_LEVEL ESP_LOG_DEBUG
#include "esp_log.h"

// In app_main():
esp_log_level_set("ADXL345", ESP_LOG_DEBUG);
```

#### I2C Bus Scanning
```c
void i2c_scan(adxl345_handle_t *handle)
{
    printf("Scanning I2C bus...\n");
    for (uint8_t addr = 0x08; addr < 0x78; addr++) {
        uint8_t dummy;
        if (i2c_master_transmit_receive(handle->dev_handle, &addr, 0, &dummy, 1, 100) == ESP_OK) {
            printf("Device found at address 0x%02X\n", addr);
        }
    }
}
```

### Performance Optimization

#### High-Speed Reading
```c
// Use higher I2C clock speed
.clk_speed = 1000000,  // 1MHz

// Read multiple samples in burst
adxl345_accel_data_t samples[10];
for (int i = 0; i < 10; i++) {
    adxl345_read_acceleration(handle, &samples[i]);
}
```

#### Low-Power Operation
```c
// Use lower data rate
adxl345_set_data_rate(handle, ADXL345_BW_RATE_25HZ);

// Enable sleep mode when not needed
adxl345_disable_measurement(handle);
// ... sleep
adxl345_enable_measurement(handle);
```

## Register Map Reference

| Register | Address | Description | R/W |
|----------|---------|-------------|-----|
| DEVID | 0x00 | Device ID (0xE5) | R |
| THRESH_TAP | 0x1D | Tap threshold | R/W |
| OFSX/Y/Z | 0x1E-0x20 | Offset calibration | R/W |
| DUR | 0x21 | Tap duration | R/W |
| LATENT | 0x22 | Tap latency | R/W |
| WINDOW | 0x23 | Tap window | R/W |
| THRESH_ACT | 0x24 | Activity threshold | R/W |
| THRESH_INACT | 0x25 | Inactivity threshold | R/W |
| TIME_INACT | 0x26 | Inactivity time | R/W |
| ACT_INACT_CTL | 0x27 | Activity/Inactivity control | R/W |
| THRESH_FF | 0x28 | Free fall threshold | R/W |
| TIME_FF | 0x29 | Free fall time | R/W |
| TAP_AXES | 0x2A | Tap axes enable | R/W |
| ACT_TAP_STATUS | 0x2B | Activity/Tap status | R |
| BW_RATE | 0x2C | Data rate control | R/W |
| POWER_CTL | 0x2D | Power control | R/W |
| INT_ENABLE | 0x2E | Interrupt enable | R/W |
| INT_MAP | 0x2F | Interrupt mapping | R/W |
| INT_SOURCE | 0x30 | Interrupt source | R |
| DATA_FORMAT | 0x31 | Data format control | R/W |
| DATAX0/1 | 0x32-0x33 | X-axis data | R |
| DATAY0/1 | 0x34-0x35 | Y-axis data | R |
| DATAZ0/1 | 0x36-0x37 | Z-axis data | R |
| FIFO_CTL | 0x38 | FIFO control | R/W |
| FIFO_STATUS | 0x39 | FIFO status | R |

## License

This driver is provided under the MIT License. See project repository for full license text.

## Contributing

Contributions are welcome! Please:
1. Fork the repository
2. Create a feature branch
3. Add tests for new functionality
4. Submit a pull request

## Support

For issues and questions:
1. Check this documentation
2. Search existing issues in the repository
3. Create a new issue with detailed description
4. Include hardware setup and error logs

[1]: https://claude.ai/chat/dc17d739-a1fe-444d-803d-e7c8c83bf253 "Project Prompts"
