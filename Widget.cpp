#include "Widget.h"
#include "Libraries/pico-displayDrivs/gfx/gfx.h"
#include "Parameters.h"
#include <cstdint>
#include <cstdio>

Widget::Widget()
    : area(nullptr)
{
}

Widget::~Widget()
{
    if (area) {
        delete area;
        area = nullptr;
    }
}

NavigableWidget::NavigableWidget()
    : Widget(), right(nullptr), left(nullptr), up(nullptr), down(nullptr)
{
}

NavigableWidget::~NavigableWidget()
{
}

void NavigableWidget::buttonPressed()
{
    // Default behavior: repaint own area if present
    if (area) {
        area->Paint();
    }
}

SensorWidget::SensorWidget(QUANTITY t)
    : NavigableWidget(), iconBitmap(nullptr), units(nullptr), type(t), title(nullptr)
{
}

SensorWidget::~SensorWidget()
{
    if (title) {
        delete title;
        title = nullptr;
    }
}

void SensorWidget::showValue(int32_t value)
{
    if (!area) return;

    // Simple display: print numeric value at area's position.
    // Adjust text color/background if needed elsewhere before calling.
    GFX_setCursor((int16_t)area->posX, (int16_t)area->posY);
    // Use simple formatting; GFX_printf(textsize, format, ...) behaves like printf
    GFX_printf(PARAM_DEFAULT_TEXT_SIZE, "%ld", (long)value);

    // If units are provided, print them after a space
    if (units) {
        GFX_printf(PARAM_DEFAULT_TEXT_SIZE, " %s", units);
    }
}

void SensorWidget::update()
{
    if (area) {
        // repaint sensor area
        area->Paint();

        // Create title lazily based on quantity type
        if (!title) {
            const wchar_t* label = L"Sensor";
            switch (type) {
                case QUANTITY_TEMPERATURE: label = L"26,8 c"; break;
                case QUANTITY_HUMIDITY:    label = L"78,8 %"; break;
                case QUANTITY_CO2:         label = L"1523ppm"; break;
                case QUANTITY_PRESSURE:    label = L"26.26 hPa"; break;
                default:                   label = L"Sensor"; break;
            }
            title = new Text(label);
            title->backgroundColor = area->backgroundColor;
            title->textSize = SENSOR_WIDGET_TEXT_SIZE;
        }

        // Style and position the title within the area
        title->backgroundColor = area->backgroundColor;
        title->color = area->color;
        title->posX = area->posX + SENSOR_WIDGET_PADDING;
        title->posY = area->posY + SENSOR_WIDGET_HEIGHT - SENSOR_WIDGET_PADDING - 3 * 8;

        // Paint the title
        title->Paint();
    }
}

SettingsWidget::SettingsWidget()
    : Widget(), title(nullptr)
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
    if (title) {
        delete title;
        title = nullptr;
    }
}

void SettingsWidget::update()
{
    if (area) {
        // repaint settings area
        area->Paint();

        // Create title lazily and configure it
        if (!title) {
            title = new Text(L"Settings");
            // title->textSize stays default unless changed here
        }

        // Style and position the title within the area
        title->backgroundColor = area->backgroundColor;
        title->color = area->color;
        title->posX = area->posX + PARAM_DEFAULT_PADDING;
        title->posY = area->posY + PARAM_DEFAULT_PADDING;

        // Paint the title
        title->Paint();
    }
}

void Widget::update()
{
    // Default update behaviour: repaint own area if present
    if (area) {
        area->Paint();
    }
}
