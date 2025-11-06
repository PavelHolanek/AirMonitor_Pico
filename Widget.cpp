#include "Widget.h"
#include "Libraries/pico-displayDrivs/gfx/gfx.h"
#include "Parameters.h"
#include <cstdint>
#include <cstdio>
#include <wchar.h>
#include <cstring>

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
    : NavigableWidget(),
      iconBitmap(nullptr),
      units(nullptr),
      type(t),
      pictogram(nullptr),
      arrow(nullptr),
      valueText(nullptr),
      unitsText(nullptr),
      childrenAdded(false)
{
    valueBuffer[0] = L'\0';
    unitsBuffer[0] = L'\0';

    // Set default units per quantity
    switch (type) {
        case QUANTITY_TEMPERATURE: units = "C";   break;
        case QUANTITY_HUMIDITY:    units = "%";   break;
        case QUANTITY_CO2:         units = "ppm"; break;
        case QUANTITY_PRESSURE:    units = "hPa"; break;
        default:                   units = "";    break;
    }
}

SensorWidget::~SensorWidget()
{
    if (pictogram) { delete pictogram; pictogram = nullptr; }
    if (arrow) { delete arrow; arrow = nullptr; }
    if (valueText) { delete valueText; valueText = nullptr; }
    if (unitsText) { delete unitsText; unitsText = nullptr; }
}

void SensorWidget::showValue(int32_t value)
{
    // Update the internal buffer; actual drawing occurs in update()
    swprintf(valueBuffer, sizeof(valueBuffer) / sizeof(valueBuffer[0]), L"%ld", (long)value);
    if (valueText) {
        valueText->str = valueBuffer;
    }
}

void SensorWidget::update()
{
    if (!area) return;

    // Lazy initialize children
    if (!pictogram) pictogram = new bitMap32();
    if (!arrow) arrow = new bitMap32();
    if (!valueText) valueText = new Text(L"");
    if (!unitsText) unitsText = new Text(L"");

    // Convert units (ASCII) into wide buffer
    unitsBuffer[0] = L'\0';
    if (units) {
        size_t i = 0;
        size_t maxn = sizeof(unitsBuffer) / sizeof(unitsBuffer[0]) - 1;
        while (units[i] != '\0' && i < maxn) {
            unitsBuffer[i] = (wchar_t)units[i];
            ++i;
        }
        unitsBuffer[i] = L'\0';
    }
    unitsText->str = unitsBuffer;

    // Assign bitmap data pointers
    pictogram->data = iconBitmap; // pictogram from external source if provided

    // Colors and sizes from parent area
    Color bg = area->backgroundColor;
    Color fg = area->color;

    pictogram->backgroundColor = bg;
    pictogram->color = fg;

    arrow->backgroundColor = bg;
    arrow->color = fg;

    valueText->backgroundColor = bg;
    valueText->color = fg;
    valueText->textSize = 3;

    unitsText->backgroundColor = bg;
    unitsText->color = fg;
    unitsText->textSize = 2;

    // Layout calculations
    const uint16_t ax = area->posX;
    const uint16_t ay = area->posY;
    const uint16_t aw = area->sizeX;
    const uint16_t ah = area->sizeY;

    // UL pictogram (assume 32x32)
    pictogram->posX = ax + SENSOR_WIDGET_PADDING;
    pictogram->posY = ay + SENSOR_WIDGET_PADDING;

    // UR arrow (assume 32x32)
    arrow->posX = ax + aw - SENSOR_WIDGET_PADDING - 32;
    arrow->posY = ay + SENSOR_WIDGET_PADDING;

    // BL value
    valueText->posX = ax + SENSOR_WIDGET_PADDING;
    valueText->posY = ay + ah - SENSOR_WIDGET_PADDING - (uint16_t)(8 * valueText->textSize);

    // BR units right-aligned (6 px per char at size 1)
    size_t ulen = 0;
    while (unitsBuffer[ulen] != L'\0' && ulen < (sizeof(unitsBuffer) / sizeof(unitsBuffer[0]))) ++ulen;
    const uint16_t unitsPixelWidth = (uint16_t)(ulen * 6 * unitsText->textSize);
    unitsText->posX = ax + aw - SENSOR_WIDGET_PADDING - unitsPixelWidth;
    unitsText->posY = ay + ah - SENSOR_WIDGET_PADDING - (uint16_t)(8 * unitsText->textSize);

    // Add as children once so Area will paint them
    if (!childrenAdded) {
        area->addChildren(pictogram);
        area->addChildren(arrow);
        area->addChildren(valueText);
        area->addChildren(unitsText);
        childrenAdded = true;
    }

    // Paint area and contained elements
    area->Paint();
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
