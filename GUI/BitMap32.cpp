#include "GraphicElement.h"
#include "Libraries/pico-displayDrivs/gfx/gfx.h"
#include "Bitmaps.h"

bitMap32::bitMap32() {
    data = nullptr;
}

bitMap32::~bitMap32() {}

void bitMap32::Paint()
{
    const uint8_t (*bitmap)[128];
    if (data)
    {
        bitmap = data;
    }
    else
    {
        bitmap = &BITMAP32_SOLID;
    }
    for (int y = 0; y < 32; ++y) 
    {
        const uint8_t* row = &(*bitmap)[y * 4];
        for (int xb = 0; xb < 4; ++xb) 
        {
            uint8_t b = row[xb];
            for (int bit = 0; bit < 8; ++bit) 
            {
                if (b & (1u << (7 - bit))) 
                {
                    int x = xb * 8 + bit;
                    GFX_drawPixel((int16_t)(posX + x), (int16_t)(posY + y), color);
                }
            }
        }
    }
}

