#include "GraphicElement.h"
#include "Libraries/pico-displayDrivs/gfx/gfx.h"

Line::Line() : endPosX(0), endPosY(0) {}
Line::~Line() {}

void Line::Paint()
{
    GFX_drawLine(posX, posY, endPosX, endPosY, color);
}

