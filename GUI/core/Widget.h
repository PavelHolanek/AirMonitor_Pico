#ifndef WIDGET_H
#define WIDGET_H

#include "GraphicElement.h"

// Base non-navigable widget
class Widget
{
public:
    Area* area;
    Widget();
    virtual ~Widget();

    // Default update repaints the widget's area
    virtual void update();
};

#endif // WIDGET_H