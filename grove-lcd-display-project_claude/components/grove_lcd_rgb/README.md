# Grove LCD RGB Component

這是一個完整的ESP-IDF組件，用於控制Grove LCD RGB背光模組。

## 文件結構

```
components/
└── grove_lcd_rgb/
    ├── include/
    │   └── grove_lcd_rgb.h
    ├── src/
    │   └── grove_lcd_rgb.c
    ├── CMakeLists.txt
    └── Kconfig
```

## 使用方法

1. 將 `grove_lcd_rgb` 文件夾放在您的項目的 `components/` 目錄中
2. 在您的 `main/CMakeLists.txt` 中添加依賴：
   ```cmake
   idf_component_register(SRCS "main.c"
                       INCLUDE_DIRS "."
                       REQUIRES grove_lcd_rgb)
   ```
3. 在您的程式中包含標頭文件：
   ```c
   #include "grove_lcd_rgb.h"
   ```

## 配置

可以通過 `idf.py menuconfig` 在 "Grove LCD RGB Configuration" 選單中配置I2C引腳。