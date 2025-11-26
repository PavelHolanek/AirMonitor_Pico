#ifndef WIDGET_H
#define WIDGET_H

#include "GraphicElement.h"

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
};

class SensorWidget : public NavigableWidget
{
public:
    SensorWidget(QUANTITY t);
    virtual ~SensorWidget();

    void showValue(int32_t value);
    void update() override;

    uint32_t (*iconBitmap)[32];

    const char *units;
    QUANTITY type;

private:
    bitMap32* pictogram;
    bitMap32* arrow;
    Text* valueText;
    Text* unitsText;
    bool childrenAdded;
    wchar_t valueBuffer[16];
    wchar_t unitsBuffer[12];
};

class SettingsWidget : public Widget
{
public:
    SettingsWidget();
    virtual ~SettingsWidget();
    void update() override;
    void buttonPressed();
    void leftMove();
    void rightMove();
private:
    Text* title;
};
#endif // WIDGET_H
