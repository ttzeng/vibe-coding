# Grove LCD RGB Component for ESP-IDF

這是一個為ESP-IDF框架設計的Grove LCD RGB背光模組驅動組件，使用最新的I2C master API。

## 特性

- 支援Grove LCD RGB背光模組 (16x2字符LCD)
- 使用ESP-IDF最新的I2C master API
- 可配置的GPIO引腳
- RGB背光顏色控制
- 完整的LCD控制功能
- 線程安全設計
- 錯誤處理和日誌記錄

## 硬體需求

- ESP32-C3 Super Mini (或其他ESP32系列)
- Grove LCD RGB背光模組
- 4條杜邦線 (VCC, GND, SCL, SDA)

## 安裝

### 1. 項目結構

```
your_project/
├── components/
│   └── grove_lcd_rgb/
│       ├── include/
│       │   └── grove_lcd_rgb.h
│       ├── src/
│       │   └── grove_lcd_rgb.c
│       ├── CMakeLists.txt
│       └── Kconfig
├── main/
│   ├── main.c
│   └── CMakeLists.txt
├── CMakeLists.txt
└── sdkconfig
```

### 2. 安裝組件

將 `grove_lcd_rgb` 文件夾放入您的項目的 `components/` 目錄中。

### 3. 配置

使用 menuconfig 配置GPIO引腳：

```bash
idf.py menuconfig
```

導航到 "Grove LCD RGB Configuration" 選單配置：
- SCL GPIO Pin (預設: GPIO9 for ESP32-C3 Super Mini)
- SDA GPIO Pin (預設: GPIO8 for ESP32-C3 Super Mini)
- I2C Clock Frequency (預設: 100kHz)

## 硬體連接

### ESP32-C3 Super Mini 連接

| Grove LCD RGB | ESP32-C3 Super Mini |
|---------------|---------------------|
| VCC           | 3.3V               |
| GND           | GND                |
| SCL           | GPIO9              |
| SDA           | GPIO8              |

## API 參考

### 初始化和清理

#### `grove_lcd_rgb_init()`
```c
esp_err_t grove_lcd_rgb_init(const grove_lcd_rgb_config_t *config, grove_lcd_rgb_handle_t *handle);
```
初始化Grove LCD RGB模組。

#### `grove_lcd_rgb_deinit()`
```c
esp_err_t grove_lcd_rgb_deinit(grove_lcd_rgb_handle_t handle);
```
清理並釋放資源。

### 顯示控制

#### `grove_lcd_rgb_clear()`
```c
esp_err_t grove_lcd_rgb_clear(grove_lcd_rgb_handle_t handle);
```
清除LCD顯示。

#### `grove_lcd_rgb_set_cursor()`
```c
esp_err_t grove_lcd_rgb_set_cursor(grove_lcd_rgb_handle_t handle, uint8_t col, uint8_t row);
```
設定游標位置 (col: 0-15, row: 0-1)。

#### `grove_lcd_rgb_print()`
```c
esp_err_t grove_lcd_rgb_print(grove_lcd_rgb_handle_t handle, const char *str);
```
印出字串到LCD。

#### `grove_lcd_rgb_printf()`
```c
esp_err_t grove_lcd_rgb_printf(grove_lcd_rgb_handle_t handle, const char *format, ...);
```
格式化印出到LCD (類似printf)。

### 背光控制

#### `grove_lcd_rgb_set_color()`
```c
esp_err_t grove_lcd_rgb_set_color(grove_lcd_rgb_handle_t handle, grove_lcd_rgb_color_t color);
```
設定RGB背光顏色。

#### `grove_lcd_rgb_set_color_rgb()`
```c
esp_err_t grove_lcd_rgb_set_color_rgb(grove_lcd_rgb_handle_t handle, uint8_t red, uint8_t green, uint8_t blue);
```
使用個別RGB值設定背光顏色。

### 預定義顏色

```c
grove_lcd_rgb_color_t red = GROVE_LCD_COLOR_RED;
grove_lcd_rgb_color_t green = GROVE_LCD_COLOR_GREEN;
grove_lcd_rgb_color_t blue = GROVE_LCD_COLOR_BLUE;
grove_lcd_rgb_color_t yellow = GROVE_LCD_COLOR_YELLOW;
grove_lcd_rgb_color_t magenta = GROVE_LCD_COLOR_MAGENTA;
grove_lcd_rgb_color_t cyan = GROVE_LCD_COLOR_CYAN;
grove_lcd_rgb_color_t white = GROVE_LCD_COLOR_WHITE;
grove_lcd_rgb_color_t black = GROVE_LCD_COLOR_BLACK;
```

## 使用範例

### 基本使用

```c
#include "grove_lcd_rgb.h"

void app_main(void)
{
    // 使用預設配置初始化
    grove_lcd_rgb_config_t config = GROVE_LCD_RGB_CONFIG_DEFAULT();
    grove_lcd_rgb_handle_t lcd;
    
    ESP_ERROR_CHECK(grove_lcd_rgb_init(&config, &lcd));
    
    // 設定藍色背光
    grove_lcd_rgb_color_t blue = GROVE_LCD_COLOR_BLUE;
    grove_lcd_rgb_set_color(lcd, blue);
    
    // 顯示文字
    grove_lcd_rgb_clear(lcd);
    grove_lcd_rgb_set_cursor(lcd, 0, 0);
    grove_lcd_rgb_print(lcd, "Hello World");
    grove_lcd_rgb_set_cursor(lcd, 0, 1);
    grove_lcd_rgb_print(lcd, "ESP32-C3");
}
```

### 自定義配置

```c
grove_lcd_rgb_config_t config = {
    .scl_io_num = 9,
    .sda_io_num = 8,
    .clk_speed_hz = 100000,
    .i2c_port = I2C_NUM_0,
};

grove_lcd_rgb_handle_t lcd;
ESP_ERROR_CHECK(grove_lcd_rgb_init(&config, &lcd));
```

### 動態顯示

```c
int counter = 0;
while (1) {
    grove_lcd_rgb_clear(lcd);
    grove_lcd_rgb_set_cursor(lcd, 0, 0);
    grove_lcd_rgb_printf(lcd, "Count: %d", counter++);
    
    // 更換顏色
    grove_lcd_rgb_set_color_rgb(lcd, 255, 0, 0); // 紅色
    vTaskDelay(pdMS_TO_TICKS(1000));
}
```

## 故障排除

### 常見問題

1. **LCD 無顯示**
   - 檢查硬體連接
   - 確認GPIO引腳配置
   - 檢查電源供應 (3.3V)

2. **背光不亮**
   - RGB控制器可能需要重新初始化
   - 檢查I2C地址是否正確

3. **字符顯示異常**
   - 確認使用UTF-8編碼
   - 檢查字符是否在支援範圍內

### 調試

啟用詳細日誌：
```c
esp_log_level_set("grove_lcd_rgb", ESP_LOG_VERBOSE);
```

## 許可證

此組件基於ESP-IDF的許可證條款提供。

## 貢獻

歡迎提交問題報告和拉取請求來改進此組件。