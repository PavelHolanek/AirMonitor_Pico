#include "GraphicElement.h"
#include "Libraries/pico-displayDrivs/gfx/gfx.h"
#include <wchar.h>
#include "Parameters.h"
#include "Bitmaps.h"
#include <cstdio>

GraphicElement::GraphicElement() : backgroundColor(PARAM_COLOR_BLACK), color(PARAM_COLOR_BLACK) { posX = 0; posY = 0; }
GraphicElement::GraphicElement(uint16_t x, uint16_t y) : backgroundColor(PARAM_COLOR_BLACK), color(PARAM_COLOR_BLACK), posX(x), posY(y) {}
GraphicElement::~GraphicElement() {}

Area::Area() : sizeX(0), sizeY(0), numberOfChildren(0)
{
    for (uint8_t i = 0; i < MAX_AREA_CHILDREN_COUNT; ++i)
    {
        children[i] = nullptr;
    }
}

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

Area::Area(uint16_t x, uint16_t y, uint16_t sx, uint16_t sy) : GraphicElement(x, y), sizeX(sx), sizeY(sy), numberOfChildren(0)
{
    for (uint8_t i = 0; i < MAX_AREA_CHILDREN_COUNT; ++i)
    {
        children[i] = nullptr;
    }
}
Area::~Area() {}

Text::Text(const wchar_t* s) : GraphicElement(), str(s), textSize(1) {}
Text::Text(const wchar_t* s, uint16_t x, uint16_t y) : GraphicElement(x, y), str(s), textSize(1) {}
Text::~Text() {}

Line::Line() : endPosX(0), endPosY(0) {}
Line::~Line() {}

bitMap32::bitMap32() {
    data = nullptr;
}
bitMap32::~bitMap32() {}

bitMap48::bitMap48() {
    data = nullptr;
}
bitMap48::~bitMap48() {}

void Area::addChildren(GraphicElement* child)
{
    if (!child || child == this)
    {
        return;
    }
    if (numberOfChildren >= MAX_AREA_CHILDREN_COUNT)
    {
        return;
    }
    children[numberOfChildren] = child;
    child->backgroundColor = this->backgroundColor;
    numberOfChildren++;
}

void Area::Paint()
{
    GFX_fillRoundedRect(posX, posY, sizeX, sizeY, SENSOR_WIDGET_CORNER_RADIUS, backgroundColor);
    
    for (uint8_t i = 0; i < numberOfChildren; ++i)
    {
        if (children[i])
        {
            children[i]->Paint();
        }
    }
}

void Text::Paint() 
{
    size_t i = 0;
    size_t len = wcslen(str);
    char charArray[len + 1];
    while (str[i] != L'\0'  && i < len)
    {
        if (str[i] > 127)
        {
            charArray[i] = getCharForWideChar(str[i]);
        }
        else
        {
            charArray[i] = str[i];
        }
        i++;
    }

    charArray[i] = '\0';
    GFX_setCursor(posX, posY);
    GFX_setTextColor(color);
    GFX_setTextBack(backgroundColor);
    GFX_printf(textSize, charArray);
}

void Line::Paint()
{
    GFX_drawLine(posX, posY, endPosX, endPosY, color);
}

void bitMap32::Paint()
{
    const uint8_t (*bitmap)[128];
    if (data) {
        bitmap = data;
    } else {
        bitmap = &BITMAP32_SOLID;
    }
    for (int y = 0; y < 32; ++y) {
        const uint8_t* row = &(*bitmap)[y * 4];
        for (int xb = 0; xb < 4; ++xb) {
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
