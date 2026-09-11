#ifndef SETTINGS_ICON_WIDGET_H
#define SETTINGS_ICON_WIDGET_H

#include "core/NavigableWidget.h"
#include "core/GraphicElement.h"

class SettingsIconWidget : public NavigableWidget
{
public:
    SettingsIconWidget();
    virtual ~SettingsIconWidget();
    void update() override;
    void buttonPressed();
    void leftMove();
    void rightMove();
    void selected() override;
    void deselected() override;
private:
    bitMap48* icon;
};

#endif // SETTINGS_ICON_WIDGET_H

