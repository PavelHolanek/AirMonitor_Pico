// SettingWindow.h - settings screen window
#ifndef SETTING_WINDOW_H
#define SETTING_WINDOW_H

#include "Window.h"

#define SETTINGS_ROWS_COUNT 5

class SettingWindow : public Window
{
public:
    SettingWindow();
    virtual ~SettingWindow() {}

    void enterMainWindow();

    NavigableWidget settingsRows[SETTINGS_ROWS_COUNT];
};

#endif // SETTING_WINDOW_H

