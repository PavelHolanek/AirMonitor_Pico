#ifndef SETTINGS_WIDGET_H
#define SETTINGS_WIDGET_H

#include "NavigableWidget.h"
#include "GraphicElement.h"

class SettingsWidget : public NavigableWidget
{
public:
    SettingsWidget();
    virtual ~SettingsWidget();
    void update() override;
    void buttonPressed();
    void leftMove();
    void rightMove();
    void selected() override;
    void deselected() override;
private:
    bitMap48* icon;
};

#endif // SETTINGS_WIDGET_H

