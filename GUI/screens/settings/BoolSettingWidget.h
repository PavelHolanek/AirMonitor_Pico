// BoolSettingWidget.h - settings row holding an on/off value
#ifndef BOOL_SETTING_WIDGET_H
#define BOOL_SETTING_WIDGET_H

#include "SettingWidget.h"

class BoolSettingWidget : public SettingWidget
{
private:
    bool *value;

public:
    BoolSettingWidget(char* title, bool *value);
    virtual ~BoolSettingWidget();

    void update() override;
    void applySetting() override;
};

#endif // BOOL_SETTING_WIDGET_H
