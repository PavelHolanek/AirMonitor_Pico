// BoolSettingWidget.cpp - title on the left, check / cross on the right
#include "BoolSettingWidget.h"
#include "Parameters.h"
#include "core/Bitmaps.h"

BoolSettingWidget::BoolSettingWidget(const wchar_t* title, bool* value)
    : SettingWidget(title), value(value)
{
}

BoolSettingWidget::~BoolSettingWidget()
{
}

void BoolSettingWidget::update()
{
    if (!area)
    {
        return;
    }

    area->Paint();
    drawTitle();

    bitMap32 icon;
    icon.data = (uint8_t (*)[128])((value && *value) ? &BITMAP32_CHECK : &BITMAP32_CROSS);
    icon.color = area->color;
    icon.backgroundColor = area->backgroundColor;
    icon.posX = area->getEndX() - SETTINGS_ROW_PADDING - SETTINGS_ICON_SIZE;
    icon.posY = area->posY + (SETTINGS_ROW_HEIGHT - SETTINGS_ICON_SIZE) / 2;
    icon.Paint();
}

void BoolSettingWidget::applySetting()
{
    if (!value)
    {
        return;
    }
    *value = !*value;
    update();
}
