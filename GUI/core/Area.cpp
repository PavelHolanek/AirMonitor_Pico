#include "GraphicElement.h"
#include "Libraries/pico-displayDrivs/gfx/gfx.h"
#include "Parameters.h"

Area::Area() : sizeX(0), sizeY(0), numberOfChildren(0)
{
    for (uint8_t i = 0; i < MAX_AREA_CHILDREN_COUNT; ++i)
    {
        children[i] = nullptr;
    }
}

Area::Area(uint16_t x, uint16_t y, uint16_t sx, uint16_t sy)
    : GraphicElement(x, y), sizeX(sx), sizeY(sy), numberOfChildren(0)
{
    for (uint8_t i = 0; i < MAX_AREA_CHILDREN_COUNT; ++i)
    {
        children[i] = nullptr;
    }
}

Area::~Area() {}

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

