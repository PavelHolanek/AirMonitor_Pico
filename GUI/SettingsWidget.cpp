#include "SettingsWidget.h"
#include "Parameters.h"
#include "Bitmaps.h"

SettingsWidget::SettingsWidget()
    : NavigableWidget(), icon(nullptr)
{
}

void SettingsWidget::buttonPressed()
{
    // Placeholder for settings widget button action
    if (area) area->Paint();
}

void SettingsWidget::leftMove()
{
    // Placeholder left movement in settings
    if (area) area->Paint();
}

void SettingsWidget::rightMove()
{
    // Placeholder right movement in settings
    if (area) area->Paint();
}

SettingsWidget::~SettingsWidget()
{
    if (icon) {
        delete icon;
        icon = nullptr;
    }
}

void SettingsWidget::update()
{
    if (!area) return;

    // repaint settings area background
    area->Paint();

    // Create icon lazily and configure it
    if (!icon) {
        icon = new bitMap48();
        // default to settings gear bitmap from Bitmaps.h
        icon->data = (uint8_t (*)[288])&BITMAP48_SETTINGS;
    }

    // Style and position the icon within the area
    icon->backgroundColor = area->backgroundColor;
    icon->color = area->color;

    const uint16_t ax = area->posX;
    const uint16_t ay = area->posY;
    // Keep layout similar to QuantityWidget: top-left with padding
    icon->posX = ax -3;
    icon->posY = ay + SENSOR_WIDGET_PADDING - 10;

    // Paint the icon
    icon->Paint();
}

void SettingsWidget::selected()
{
    area->backgroundColor = PARAM_COLOR_GRAY_1;
    area->color = PARAM_COLOR_BLACK;
    update();
}
void SettingsWidget::deselected()
{
    area->backgroundColor = PARAM_COLOR_BLACK;
    area->color = PARAM_COLOR_WHITE;
    update();
}

