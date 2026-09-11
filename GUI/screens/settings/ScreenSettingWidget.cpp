// ScreenSettingWidget.cpp - a row that just leaves for another window
#include "ScreenSettingWidget.h"
#include "GUIManager.h"

ScreenSettingWidget::ScreenSettingWidget(const wchar_t* title, Window* window)
    : SettingWidget(title), window(window)
{
}

ScreenSettingWidget::~ScreenSettingWidget()
{
}

void ScreenSettingWidget::update()
{
    if (!area)
    {
        return;
    }

    area->Paint();
    drawTitle();
}

void ScreenSettingWidget::applySetting()
{
    if (window)
    {
        gui_changeWindow(window);
    }
}
