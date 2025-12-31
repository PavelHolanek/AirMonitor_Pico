#include "Widget.h"

Widget::Widget()
    : area(nullptr)
{
}

Widget::~Widget()
{
    if (area) {
        delete area;
        area = nullptr;
    }
}

void Widget::update()
{
    // Default update behaviour: repaint own area if present
    if (area) {
        area->Paint();
    }
}