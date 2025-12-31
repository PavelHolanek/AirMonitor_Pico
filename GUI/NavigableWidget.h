#ifndef NAVIGABLE_WIDGET_H
#define NAVIGABLE_WIDGET_H

#include "Widget.h"

class NavigableWidget : public Widget
{
public:
    NavigableWidget();
    virtual ~NavigableWidget();

    NavigableWidget* right;
    NavigableWidget* left;
    NavigableWidget* up;
    NavigableWidget* down;

    void buttonPressed();
    virtual void selected();
    virtual void deselected();
};

#endif // NAVIGABLE_WIDGET_H

