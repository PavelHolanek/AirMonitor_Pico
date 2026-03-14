#include "QuantityWidget.h"
#include "Parameters.h"
#include "Bitmaps.h"
#include "Libraries/pico-displayDrivs/gfx/gfx.h"
#include <cstdio>

QuantityWidget::QuantityWidget(QUANTITY t)
    : NavigableWidget(),
      iconBitmap(nullptr),
      units(nullptr),
      type(t),
      pictogram(nullptr),
      arrow(nullptr),
      valueText(nullptr),
      unitsText(nullptr),
      childrenAdded(false),
      initialized(false)
{
    valueBuffer[0] = L'\0';
    unitsBuffer[0] = L'\0';

    valueText = new Text(L"--");

    // Set default units per quantity
    switch (type)
    {
        case QUANTITY_TEMPERATURE: units = "C";   break;
        case QUANTITY_HUMIDITY:    units = "%%";  break; // match original behavior
        case QUANTITY_CO2:         units = "ppm"; break;
        case QUANTITY_PRESSURE:    units = "hPa"; break;
        default:                   units = "";    break;
    }
}

QuantityWidget::~QuantityWidget()
{
    if (pictogram) { delete pictogram; pictogram = nullptr; }
    if (arrow) { delete arrow; arrow = nullptr; }
    if (valueText) { delete valueText; valueText = nullptr; }
    if (unitsText) { delete unitsText; unitsText = nullptr; }
}

void QuantityWidget::selected()
{
    area->backgroundColor = PARAM_COLOR_GRAY_1;
    update();
}
void QuantityWidget::deselected()
{
    area->backgroundColor = PARAM_COLOR_BLACK;
    update();
}

void QuantityWidget::setValue(int32_t value)
{
    if (type == QUANTITY_TEMPERATURE)
    {
        int32_t v = value;
        wchar_t sign[2] = {0};
        if (v < 0)
        {
            sign[0] = L'-';
            v = -v;
        }
        long whole = v / 100;
        long frac1 = (v / 10) % 10;
        swprintf(valueBuffer,
                 sizeof(valueBuffer) / sizeof(valueBuffer[0]),
                 L"%ls%ld,%ld",
                 sign,
                 whole,
                 frac1);
    } 
    else if (type == QUANTITY_PRESSURE) 
    {
        long hpa = (long)(value / 100);
        swprintf(valueBuffer, sizeof(valueBuffer) / sizeof(valueBuffer[0]), L"%ld", hpa);
    }
    else if (type == QUANTITY_HUMIDITY)
    {
        long percent = (long)(value / 1000);
        swprintf(valueBuffer, sizeof(valueBuffer) / sizeof(valueBuffer[0]), L"%ld", percent);
    }
    else
    {
        swprintf(valueBuffer, sizeof(valueBuffer) / sizeof(valueBuffer[0]), L"%ld", (long)value);
    }

    if (valueText)
    {
        valueText->str = valueBuffer;
    }
}

void QuantityWidget::update()
{
    if (!area) return;
    if (!initialized) initialize();

    // Update only dynamic parts: colors, value text, arrow
    Color bg = area->backgroundColor;
    Color fg = area->color;

    pictogram->backgroundColor = bg;
    pictogram->color = fg;

    arrow->backgroundColor = bg;
    arrow->color = fg;

    valueText->backgroundColor = bg;
    valueText->color = fg;
    valueText->str = valueBuffer;

    unitsText->backgroundColor = bg;
    unitsText->color = fg;

    // Repaint area and children
    GFX_createFramebuf(area->posX, area->posY, area->sizeX, area->sizeY);
    GFX_fillRect(area->posX, area->posY, area->sizeX, area->sizeY, MAIN_WINDOW_BACKGROUND);
    area->Paint();
    GFX_flush();
    GFX_destroyFramebuf();
}

void QuantityWidget::initialize()
{
    if (!area || initialized) return;

    if (!pictogram) pictogram = new bitMap48();
    if (!arrow) arrow = new bitMap32();
    if (!valueText) valueText = new Text(L"--");
    if (!unitsText) unitsText = new Text(L"");

    // Convert units (ASCII) into wide buffer once
    unitsBuffer[0] = L'\0';
    if (units)
    {
        size_t i = 0;
        size_t maxn = sizeof(unitsBuffer) / sizeof(unitsBuffer[0]) - 1;
        while (units[i] != '\0' && i < maxn)
        {
            unitsBuffer[i] = (wchar_t)units[i];
            ++i;
        }
        unitsBuffer[i] = L'\0';
    }
    unitsText->str = unitsBuffer;

    // Assign bitmap data pointers once
    if (type == QUANTITY_TEMPERATURE)
    {
        pictogram->data = (uint8_t (*)[288])&BITMAP48_TEMPERATURE;
    }
    else if (type == QUANTITY_HUMIDITY)
    {
        pictogram->data = (uint8_t (*)[288])&BITMAP48_HUMIDITY;
    }
    else if (type == QUANTITY_CO2)
    {
        pictogram->data = (uint8_t (*)[288])&BITMAP48_CO2;
    }
    else if (type == QUANTITY_PRESSURE)
    {
        pictogram->data = (uint8_t (*)[288])&BITMAP48_PRESSURE;
    }
    else
    {
        pictogram->data = iconBitmap;
    }

    // Set static text sizes
    valueText->textSize = 4;
    unitsText->textSize = 3;

    // Static layout (positions and sizes won't change)
    const uint16_t ax = area->posX;
    const uint16_t ay = area->posY;
    const uint16_t aw = area->sizeX;
    const uint16_t ah = area->sizeY;

    // UL pictogram (assume 48x48)
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
    if (!childrenAdded)
    {
        area->addChildren(pictogram);
        area->addChildren(arrow);
        area->addChildren(valueText);
        area->addChildren(unitsText);
        childrenAdded = true;
    }

    initialized = true;
}
