// EnumSettingWidget.h - settings row selecting one value out of a fixed set
#ifndef ENUM_SETTING_WIDGET_H
#define ENUM_SETTING_WIDGET_H

#include <stdint.h>
#include "SettingWidget.h"

class EnumSettingWidget : public SettingWidget
{
private:
    uint8_t* value;

    const wchar_t* const* labels;

    uint8_t labelCount;

    const wchar_t* currentLabel() const;

public:
    EnumSettingWidget(const wchar_t* title, uint8_t* value,
                      const wchar_t* const* labels, uint8_t labelCount);
    virtual ~EnumSettingWidget();

    void moveLeft() override;
    void moveRight() override;

    void update() override;
    void applySetting() override;
};

#endif // ENUM_SETTING_WIDGET_H
