// components/st7735/st7735_font.h
#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

// Font dimensions
#define FONT_WIDTH  5
#define FONT_HEIGHT 8

// 5x8 bitmap font for ASCII characters 32-127
// Each character is 5 bytes, with each byte representing a column
extern const uint8_t font5x8[96][5];

#ifdef __cplusplus
}
#endif
