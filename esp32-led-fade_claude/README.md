# ESP32-C3 LED Fade In/Out Project

A complete ESP-IDF project demonstrating smooth LED brightness control using PWM (Pulse Width Modulation) on the ESP32-C3 microcontroller.

## Overview

This project creates a smooth fade-in and fade-out effect for an LED using the ESP32-C3's built-in LEDC (LED Control) peripheral. The LED gradually transitions from off to full brightness and back to off in a continuous cycle.

## Features

- Smooth LED fade transitions using PWM
- Configurable fade timing and parameters
- Software-based fade implementation with precise control
- Hardware-accelerated fade option (alternative implementation)
- Comprehensive error handling and logging
- FreeRTOS task-based architecture

## Hardware Requirements

### Components
- ESP32-C3 development board
- LED (any color, 3.3V compatible recommended)
- Current limiting resistor (220Ω - 1kΩ)
- Breadboard and jumper wires

### Circuit Diagram

```
ESP32-C3 GPIO8 ----[220Ω Resistor]----[LED Anode]
                                           |
                                      [LED Cathode]
                                           |
ESP32-C3 GND -------------------------------
```

### Pin Configuration
- **LED Pin**: GPIO8 (configurable in code)
- **Power**: 3.3V from ESP32-C3
- **Ground**: GND

## Software Requirements

- ESP-IDF v4.4 or later
- ESP-IDF toolchain for ESP32-C3

## Project Structure

```
esp32-led-fade/
├── main/
│   ├── esp32_led_fade.c    # Main application code
│   └── CMakeLists.txt      # Component build configuration
├── CMakeLists.txt          # Project build configuration
└── README.md               # This documentation
```

## Configuration Parameters

The following parameters can be adjusted in `esp32_led_fade.c`:

### PWM Configuration
```c
#define LEDC_TIMER          LEDC_TIMER_0
#define LEDC_MODE           LEDC_LOW_SPEED_MODE
#define LEDC_CHANNEL        LEDC_CHANNEL_0
#define LEDC_DUTY_RES       LEDC_TIMER_13_BIT  // 13-bit resolution (0-8191)
#define LEDC_FREQUENCY      (4000)             // 4 kHz PWM frequency
```

### Fade Timing
```c
#define FADE_TIME_MS        (3000)  // Total fade duration (3 seconds)
#define FADE_STEP_DELAY_MS  (10)    // Delay between steps (10ms)
```

## Installation and Setup

### 1. Install ESP-IDF
Follow the [ESP-IDF Getting Started Guide](https://docs.espressif.com/projects/esp-idf/en/latest/esp32c3/get-started/index.html) for your platform.

### 2. Create Project
```bash
mkdir esp32-led-fade
cd esp32-led-fade
idf.py create-project .
```

### 3. Replace Main Code
Replace the contents of `main/main.c` with the provided fade code.

### 4. Configure Target
```bash
idf.py set-target esp32c3
```

### 5. Build Project
```bash
idf.py build
```

### 6. Flash and Monitor
```bash
idf.py -p PORT flash monitor
```
Replace `PORT` with your serial port (e.g., `/dev/ttyUSB0` on Linux, `COM3` on Windows).

## Code Explanation

### Main Components

#### 1. LEDC Timer Configuration
Sets up the PWM timer with specified frequency and resolution:
```c
ledc_timer_config_t ledc_timer = {
    .speed_mode       = LEDC_MODE,
    .timer_num        = LEDC_TIMER,
    .duty_resolution  = LEDC_DUTY_RES,
    .freq_hz          = LEDC_FREQUENCY,
    .clk_cfg          = LEDC_AUTO_CLK
};
```

#### 2. LEDC Channel Configuration
Configures the GPIO pin and PWM channel:
```c
ledc_channel_config_t ledc_channel = {
    .speed_mode     = LEDC_MODE,
    .channel        = LEDC_CHANNEL,
    .timer_sel      = LEDC_TIMER,
    .gpio_num       = LED_GPIO_PIN,
    .duty           = 0,
    .hpoint         = 0
};
```

#### 3. Fade Functions
- `fade_in()`: Gradually increases brightness from 0% to 100%
- `fade_out()`: Gradually decreases brightness from 100% to 0%

#### 4. Task Management
Uses FreeRTOS tasks for continuous operation without blocking the main thread.

### PWM Duty Cycle Calculation

With 13-bit resolution:
- Maximum duty value: `(1 << 13) - 1 = 8191`
- Duty cycle percentage: `(duty_value / 8191) * 100`
- Brightness is proportional to duty cycle

## Expected Output

### Serial Monitor Output
```
I (298) LED_FADE: ESP32-C3 LED Fade Example Starting...
I (308) LED_FADE: LEDC configuration complete. Starting fade task...
I (318) LED_FADE: LED fade task created successfully
I (328) LED_FADE: Starting fade in...
I (3628) LED_FADE: Fade in complete
I (4138) LED_FADE: Starting fade out...
I (7438) LED_FADE: Fade out complete
```

### Visual Behavior
1. LED starts completely off
2. Gradually brightens over 3 seconds
3. Holds full brightness for 0.5 seconds
4. Gradually dims over 3 seconds
5. Holds off for 0.5 seconds
6. Cycle repeats continuously

## Customization Options

### Changing Fade Speed
Modify `FADE_TIME_MS` for different fade durations:
```c
#define FADE_TIME_MS (1000)  // 1 second fade
#define FADE_TIME_MS (5000)  // 5 second fade
```

### Adjusting Smoothness
Modify `FADE_STEP_DELAY_MS` for different smoothness levels:
```c
#define FADE_STEP_DELAY_MS (5)   // Smoother (more steps)
#define FADE_STEP_DELAY_MS (20)  // Less smooth (fewer steps)
```

### Using Different GPIO Pin
Change the LED pin assignment:
```c
#define LED_GPIO_PIN GPIO_NUM_2  // Use GPIO2 instead
```

### Hardware Fade Alternative
Uncomment `#define USE_HARDWARE_FADE` and modify `app_main()` to use hardware-accelerated fading for even smoother transitions.

## Troubleshooting

### Common Issues

#### LED Not Working
- Check wiring connections
- Verify correct GPIO pin configuration
- Ensure proper current limiting resistor
- Test LED with direct 3.3V connection

#### Compilation Errors
- Verify ESP-IDF version compatibility
- Check target is set to ESP32-C3: `idf.py set-target esp32c3`
- Clean and rebuild: `idf.py clean && idf.py build`

#### Serial Monitor Issues
- Check correct serial port selection
- Verify baud rate (115200 default)
- Ensure proper USB cable connection

### Debug Tips
- Monitor serial output for error messages
- Use `ESP_LOGI()` statements to track program flow
- Check power supply stability
- Verify GPIO pin availability (some pins may have restrictions)

## Performance Notes

- PWM frequency of 4kHz is above human audible range
- 13-bit resolution provides smooth brightness transitions
- Software fade allows precise timing control
- Hardware fade provides smoother transitions with less CPU overhead

## Extensions and Improvements

### Possible Enhancements
1. **Multiple LEDs**: Control multiple LEDs with different fade patterns
2. **Color Mixing**: Use RGB LEDs for color fade effects
3. **Button Control**: Add button to change fade modes
4. **WiFi Control**: Remote control via web interface
5. **Sensor Integration**: Automatic brightness based on ambient light
6. **Pattern Modes**: Different fade patterns (sine wave, exponential, etc.)

### Advanced Features
- Interrupt-based timing for more precise control
- DMA integration for high-performance applications
- Deep sleep mode between fade cycles for battery applications

## License

This project is provided as an educational example. Feel free to modify and use in your own projects.

## References

- [ESP-IDF Programming Guide](https://docs.espressif.com/projects/esp-idf/en/latest/esp32c3/)
- [ESP32-C3 Technical Reference Manual](https://www.espressif.com/sites/default/files/documentation/esp32-c3_technical_reference_manual_en.pdf)
- [LEDC Peripheral Documentation](https://docs.espressif.com/projects/esp-idf/en/latest/esp32c3/api-reference/peripherals/ledc.html)
- [FreeRTOS Documentation](https://www.freertos.org/Documentation/RTOS_book.html)