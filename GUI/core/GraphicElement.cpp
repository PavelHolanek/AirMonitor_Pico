#include "GraphicElement.h"
#include "Parameters.h"

GraphicElement::GraphicElement() : backgroundColor(PARAM_COLOR_BLACK), color(PARAM_COLOR_BLACK) { posX = 0; posY = 0; }
GraphicElement::GraphicElement(uint16_t x, uint16_t y) : backgroundColor(PARAM_COLOR_BLACK), color(PARAM_COLOR_BLACK), posX(x), posY(y) {}
GraphicElement::~GraphicElement() {}
