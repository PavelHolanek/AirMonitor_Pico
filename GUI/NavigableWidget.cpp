#include "NavigableWidget.h"
#include "Parameters.h"

NavigableWidget::NavigableWidget()
    : Widget(), right(nullptr), left(nullptr), up(nullptr), down(nullptr)
{
}

NavigableWidget::~NavigableWidget()
{
}

void NavigableWidget::buttonPressed()
{
    // Default behavior: repaint own area if present
    if (area) {
        area->Paint();
    }
}

void NavigableWidget::selected()
{
    area->backgroundColor = PARAM_COLOR_GRAY_1;
    update();
}

void NavigableWidget::deselected()
{
    area->backgroundColor = PARAM_COLOR_BLACK;
    update();
}

