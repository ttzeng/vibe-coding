# Hardware Setup Guide

## Required Components

1. **ESP32-C3 Super Mini Board**
2. **0.96" SSD1306 OLED Display (128x64)**
3. **Jumper Wires**
4. **Optional: Breadboard**

## Wiring Connections

| ESP32-C3 Super Mini | SSD1306 OLED | Description |
|---------------------|--------------|-------------|
| 3.3V                | VCC          | Power supply |
| GND                 | GND          | Ground |
| GPIO8               | SDA          | I2C Data line |
| GPIO9               | SCL          | I2C Clock line |

## Pin Configuration

- **SDA (GPIO8)**: I2C Data line with internal pull-up enabled
- **SCL (GPIO9)**: I2C Clock line with internal pull-up enabled
- **Boot Button (GPIO0)**: Used for menu navigation
- **Built-in LED (GPIO8)**: Status indicator (shared with SDA)

## Power Requirements

- **Operating Voltage**: 3.3V
- **Current Consumption**: 
  - Active: ~80-120mA
  - Deep sleep: <1mA
- **OLED Power**: ~20mA typical

## Troubleshooting

### Display Not Working
1. Check wiring connections
2. Verify I2C address (default 0x3C)
3. Check power supply voltage
4. Try different I2C frequency

### WiFi Issues
1. Check antenna connection
2. Verify WiFi credentials
3. Check signal strength
4. Try different channels

### Button Not Responsive
1. Check GPIO0 connection
2. Verify internal pull-up configuration
3. Check for hardware conflicts
