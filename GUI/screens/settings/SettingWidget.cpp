// SettingWidget.cpp - shared painting of a settings row
#include "SettingWidget.h"
#include "Parameters.h"

SettingWidget::SettingWidget(const wchar_t* title)
    : NavigableWidget(), title(title)
{
}

SettingWidget::~SettingWidget()
{
}

void SettingWidget::drawTitle()
{
    if (!area || !title)
    {
        return;
    }

    // Transient - Text only reads its members while painting, so there is no
    // reason to keep one allocated per row.
    Text text(title,
              area->posX + SETTINGS_ROW_PADDING,
              area->posY + (SETTINGS_ROW_HEIGHT - SETTINGS_ROW_TEXT_HEIGHT) / 2);
    text.textSize = SETTINGS_TITLE_TEXT_SIZE;
    text.color = area->color;
    text.backgroundColor = area->backgroundColor;
    text.Paint();
}

void SettingWidget::selected()
{
    if (!area)
    {
        return;
    }
    area->backgroundColor = PARAM_COLOR_WHITE;
    area->color = PARAM_COLOR_BLACK;
    update();
}

void SettingWidget::deselected()
{
    if (!area)
    {
        return;
    }
    area->backgroundColor = PARAM_COLOR_BLACK;
    area->color = PARAM_COLOR_WHITE;
    update();
}
