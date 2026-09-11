// ScreenSettingWidget.h - settings row leading to another screen
#ifndef SCREEN_SETTING_WIDGET_H
#define SCREEN_SETTING_WIDGET_H

#include "SettingWidget.h"
#include "../../core/Window"

class ScreenSettingWidget : public SettingWidget
{
private:
    Window* window

public:
    ScreenSettingWidget(char* title, Window* window);
    virtual ~ScreenSettingWidget();

    void update() override;
    void applySetting() override;
};

#endif // SCREEN_SETTING_WIDGET_H
