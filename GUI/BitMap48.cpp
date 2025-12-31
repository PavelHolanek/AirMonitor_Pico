#include "GraphicElement.h"
#include "Libraries/pico-displayDrivs/gfx/gfx.h"
#include "Bitmaps.h"

bitMap48::bitMap48() {
    data = nullptr;
}

bitMap48::~bitMap48() {}

void bitMap48::Paint()
{
    const uint8_t (*bitmap)[288];
    if (data) {
        bitmap = data;
    } else {
        bitmap = &BITMAP48_SOLID;
    }
    for (int y = 0; y < 48; ++y) {
        const uint8_t* row = &(*bitmap)[y * 6];
        for (int xb = 0; xb < 6; ++xb) {
            uint8_t b = row[xb];
            for (int bit = 0; bit < 8; ++bit) {
                if (b & (1u << (7 - bit))) {
                    int x = xb * 8 + bit;
                    GFX_drawPixel((int16_t)(posX + x), (int16_t)(posY + y), color);
                }
            }
        }
    }
}

