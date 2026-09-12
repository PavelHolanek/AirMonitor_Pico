// EnumSettingWidget.cpp - title on the left, current label on the right
#include "EnumSettingWidget.h"
#include "Parameters.h"
#include <wchar.h>

EnumSettingWidget::EnumSettingWidget(const wchar_t* title, uint8_t* value,
                                     const wchar_t* const* labels, uint8_t labelCount)
    : SettingWidget(title), value(value), currentValue(value ? *value : 0U),
      labels(labels), labelCount(labelCount)
{
    // Seeded from the live value, so the row opens on what is in force.
}

EnumSettingWidget::~EnumSettingWidget()
{
}

const wchar_t* EnumSettingWidget::currentLabel() const
{
    if (!value || !labels || labelCount == 0U || currentValue >= labelCount)
    {
        return L"?";
    }
    return labels[currentValue];
}

void EnumSettingWidget::update()
{
    if (!area)
    {
        return;
    }

    area->Paint();
    drawTitle();

    const wchar_t* label = currentLabel();

    // Right aligned against the same padding the bool icon uses, so values in
    // neighbouring rows line up on their right edge.
    Text text(label, 0U, area->posY + (SETTINGS_ROW_HEIGHT - SETTINGS_ROW_TEXT_HEIGHT) / 2);
    text.posX = area->getEndX() - SETTINGS_ROW_PADDING
                - (uint16_t)(wcslen(label) * SETTINGS_ROW_TEXT_WIDTH);
    text.textSize = SETTINGS_TITLE_TEXT_SIZE;
    text.color = area->color;
    text.backgroundColor = area->backgroundColor;
    text.Paint();
}

void EnumSettingWidget::moveRight()
{
    if (!value || labelCount == 0U)
    {
        return;
    }
    currentValue = (uint8_t)((currentValue + 1U) % labelCount);
    update();
}

void EnumSettingWidget::moveLeft()
{
    if (!value || labelCount == 0U)
    {
        return;
    }
    currentValue = (currentValue == 0U) ? (uint8_t)(labelCount - 1U)
                                        : (uint8_t)(currentValue - 1U);
    update();
}

void EnumSettingWidget::applySetting()
{
    if (value)
    {
        *value = currentValue;
    }
}
