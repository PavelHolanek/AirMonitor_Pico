// SettingWidget.h - abstract base of a single row on the settings screen
#ifndef SETTING_WIDGET_H
#define SETTING_WIDGET_H

#include "core/NavigableWidget.h"

class SettingWidget : public NavigableWidget
{
public:
    SettingWidget(char* title);
    virtual ~SettingWidget();

    char* title; 
    void drawTitle();

    void update() override = 0;
    virtual void applySetting() = 0;
};

#endif // SETTING_WIDGET_H
