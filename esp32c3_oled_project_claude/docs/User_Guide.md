# User Guide

## Getting Started

1. **Power On**: Connect USB cable to ESP32-C3 Super Mini
2. **Initial Setup**: Device will show startup screen
3. **WiFi Configuration**: Edit `WIFI_SSID` and `WIFI_PASS` in `app_config.h`
4. **Build and Flash**: Use `idf.py build flash monitor`

## Display Modes

### Clock Mode
- Shows current time and date
- Displays system uptime
- Automatically synced via NTP when WiFi connected

### System Info Mode
- Free heap memory
- CPU frequency
- Frame rate (FPS)
- System performance metrics

### Sensor Data Mode
- Temperature readings (simulated)
- Humidity levels (simulated)  
- Environmental data visualization
- Real-time sensor graphs

### Network Info Mode
- WiFi connection status
- Connected SSID name
- IP address information
- Signal strength (RSSI)

### Animations Mode
- Bouncing ball physics simulation
- Starfield scrolling effect
- Matrix rain animation
- Wave patterns
- Spiral animations
- Automatically cycles every 5 seconds

### Menu Mode
- Interactive configuration menu
- Navigate with boot button
- System settings and controls

## Button Controls

### Single Press
- Cycles through display modes in order:
  1. Clock → System Info → Sensors → Network → Animations → Menu

### Long Press (Menu Mode)
- Navigate up/down in menus
- Select items
- Back to previous menu

## Menu System

### Main Menu
- **Display Settings**: Brightness controls
- **Network Settings**: WiFi scan, disconnect
- **System Settings**: Info, reset, reboot
- **About**: Version and author information

### Navigation
- Single press: Move to next item
- Hold: Select current item
- Auto-timeout: Returns to clock mode after inactivity

## Configuration

### WiFi Setup
1. Edit `app_config.h`
2. Set `WIFI_SSID` and `WIFI_PASS`
3. Rebuild and flash
4. Or use menu system for runtime configuration

### Display Settings
- Adjust via menu system
- Settings stored in NVS (non-volatile storage)
- Persist across power cycles

## Troubleshooting

### Common Issues

**Display shows garbage text**
- Check I2C connections
- Verify font data integrity
- Try different I2C frequency

**No WiFi connection**
- Check credentials in `app_config.h`
- Verify network availability
- Check antenna connection

**Button not working**
- Check GPIO0 connection
- Verify pull-up configuration
- Check for bounce issues

**System crashes**
- Monitor serial output
- Check heap usage
- Verify stack sizes

### Debug Information

Enable debug logging in `menuconfig`:
```
Component config → Log output → Default log verbosity → Debug
```

View logs via serial monitor:
```bash
idf.py monitor
```

## Advanced Features

### Custom Animations
1. Add new animation type to `animation_type_t`
2. Implement animation function in `animations.c`
3. Add to animation update switch statement

### Additional Sensors
1. Add sensor definitions to `sensor_manager.h`
2. Implement reading functions in `sensor_manager.c`
3. Update display modes to show new data

### Menu Customization
1. Define new menu items in `menu_system.c`
2. Implement action functions
3. Add to menu structure arrays
