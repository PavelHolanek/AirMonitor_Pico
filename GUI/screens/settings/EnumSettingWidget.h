// EnumSettingWidget.h - settings row selecting one value out of a fixed set
#ifndef ENUM_SETTING_WIDGET_H
#define ENUM_SETTING_WIDGET_H

#include "SettingWidget.h"

class EnumSettingWidget : public SettingWidget
{
private:
    int* enumValue;    
    char** enumNames;

public:
    EnumSettingWidget(char* title, int* enumValue);
    virtual ~EnumSettingWidget();

    void update() override;
    void applySetting() override;
};

#endif // ENUM_SETTING_WIDGET_H
