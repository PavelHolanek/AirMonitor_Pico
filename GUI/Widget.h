#ifndef WIDGET_H
#define WIDGET_H

#include "GraphicElement.h"
#include "Clock.h"

class Widget
{
public:
    Area* area;
    Widget();
    virtual ~Widget();

    // Update the widget. Default implementation will call Paint()
    // on the widget's area (implemented in Widget.cpp).
    virtual void update();
};

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

class QuantityWidget : public NavigableWidget
{
public:
    QuantityWidget(QUANTITY t);
    virtual ~QuantityWidget();

    void setValue(int32_t value);
    void update() override;
    void initialize();

    uint8_t (*iconBitmap)[288];

    const char *units;
    QUANTITY type;

    void selected() override;
    void deselected() override;
private:
    bitMap48* pictogram;
    bitMap32* arrow;
    Text* valueText;
    Text* unitsText;
    bool childrenAdded;
    bool initialized;
    wchar_t valueBuffer[16];
    wchar_t unitsBuffer[12];
};

class SettingsWidget : public NavigableWidget
{
public:
    SettingsWidget();
    virtual ~SettingsWidget();
    void update() override;
    void buttonPressed();
    void leftMove();
    void rightMove();
private:
    bitMap48* icon;
};
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

#endif // WIDGET_H
