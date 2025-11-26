#ifndef BITMAPS32_H
#define BITMAPS32_H

#include <stdint.h>

/*
  32x32 monochrome bitmaps for use with bitMap32.

  Format expected by bitMap32::Paint():
    - Each row is a uint32_t.
    - Bit 31 is the left-most pixel, bit 0 is the right-most pixel.
    - Bit value 1 = draw pixel with element color; 0 = transparent.

  Usage:
    #include "Bitmaps32.h"

    bitMap32 bmp;
    bmp.posX = 10;
    bmp.posY = 20;
    bmp.color = 0xFFFF;           // example 16-bit RGB565 if used by GFX
    bmp.backgroundColor = 0x0000; // optional; area background should be painted first
    bmp.data = &BITMAP32_SOLID;   // note: pointer to array of 32 uint32_t
    bmp.Paint();
*/

// Solid 32x32 square (all pixels set)
static const uint32_t BITMAP32_SOLID[32] = {
    0xFFFFFFFFu, 0xFFFFFFFFu, 0xFFFFFFFFu, 0xFFFFFFFFu,
    0xFFFFFFFFu, 0xFFFFFFFFu, 0xFFFFFFFFu, 0xFFFFFFFFu,
    0xFFFFFFFFu, 0xFFFFFFFFu, 0xFFFFFFFFu, 0xFFFFFFFFu,
    0xFFFFFFFFu, 0xFFFFFFFFu, 0xFFFFFFFFu, 0xFFFFFFFFu,
    0xFFFFFFFFu, 0xFFFFFFFFu, 0xFFFFFFFFu, 0xFFFFFFFFu,
    0xFFFFFFFFu, 0xFFFFFFFFu, 0xFFFFFFFFu, 0xFFFFFFFFu,
    0xFFFFFFFFu, 0xFFFFFFFFu, 0xFFFFFFFFu, 0xFFFFFFFFu,
    0xFFFFFFFFu, 0xFFFFFFFFu, 0xFFFFFFFFu, 0xFFFFFFFFu
};

#endif // BITMAPS32_H
