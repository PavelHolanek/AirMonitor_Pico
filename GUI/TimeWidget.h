#ifndef TIME_WIDGET_H
#define TIME_WIDGET_H

#include <wchar.h>
#include "NavigableWidget.h"
#include "GraphicElement.h"
#include "Clock.h"

class TimeWidget : public NavigableWidget
{
public:
    TimeWidget();
    virtual ~TimeWidget();

    void update() override;
    void setTime(const Time& t);
    Time getTime() const;
    void selected() override;
    void deselected() override;

private:
    Text* timeText;
    Time time;
    wchar_t buffer[16];
};

#endif // TIME_WIDGET_H

