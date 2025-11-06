#include "GraphicElement.h"
#include "Libraries/pico-displayDrivs/gfx/gfx.h"
#include <wchar.h>
#include "Parameters.h"
#include "Bitmaps32.h"

GraphicElement::GraphicElement() : backgroundColor(0), color(0) { posX = 0; posY = 0; }
GraphicElement::GraphicElement(uint16_t x, uint16_t y) : backgroundColor(0), color(0), posX(x), posY(y) {}
GraphicElement::~GraphicElement() {}

Area::Area() : sizeX(0), sizeY(0), numberOfChildren(0)
{
    for (uint8_t i = 0; i < MAX_AREA_CHILDREN_COUNT; ++i)
    {
        children[i] = nullptr;
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
    const uint32_t (*bitmap)[32] = data ? data : &BITMAP32_SOLID;
    for (int y = 0; y < 32; ++y) {
        uint32_t row = (*bitmap)[y];
        for (int x = 0; x < 32; ++x) {
            if (row & (1u << (31 - x))) {
                GFX_drawPixel((int16_t)(posX + x), (int16_t)(posY + y), color);
            }
        }
    }
}
