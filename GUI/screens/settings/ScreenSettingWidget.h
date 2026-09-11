// ScreenSettingWidget.h - settings row leading to another screen
#ifndef SCREEN_SETTING_WIDGET_H
#define SCREEN_SETTING_WIDGET_H

#include "SettingWidget.h"
#include "core/Window.h"

class ScreenSettingWidget : public SettingWidget
{
private:
    // Not owned - the windows live for the whole run and GUIManager holds them.
    Window* window;

public:
    ScreenSettingWidget(const wchar_t* title, Window* window);
    virtual ~ScreenSettingWidget();

    void update() override;
    void applySetting() override;
};

#endif // SCREEN_SETTING_WIDGET_H
