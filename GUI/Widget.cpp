#include "Widget.h"
#include "Libraries/pico-displayDrivs/gfx/gfx.h"
#include "Parameters.h"
#include "Bitmaps.h"
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

void NavigableWidget::selected()
{
    area->backgroundColor = PARAM_COLOR_GRAY_1;
    update();
}
void NavigableWidget::deselected()
{
    area->backgroundColor = PARAM_COLOR_BLACK;
    update();
}

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
    switch (type) {
        case QUANTITY_TEMPERATURE: units = "C";   break;
        case QUANTITY_HUMIDITY:    units = "%%";   break;
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
    // Format values based on quantity type. For temperature, input is in centi-degrees C
    // and should be displayed as X,Y (comma as decimal separator) with one decimal,
    // truncating the remaining digits (e.g., 2535 -> 25,3).
    if (type == QUANTITY_TEMPERATURE) {
        int32_t v = value;
        wchar_t sign[2] = {0};
        if (v < 0) {
            sign[0] = L'-';
            // avoid UB for INT_MIN in absolute value; practical sensor ranges are safe
            v = -v;
        }
        long whole = v / 100;          // centi-degrees to whole degrees
        long frac1 = (v / 10) % 10;     // first decimal digit (truncated)
        swprintf(valueBuffer,
                 sizeof(valueBuffer) / sizeof(valueBuffer[0]),
                 L"%ls%ld,%ld",
                 sign,
                 whole,
                 frac1);
    } else if (type == QUANTITY_PRESSURE) {
        // Pressure value provided in Pa; display hPa as integer by truncation.
        // Example: 101563 Pa -> "1015"; 99789 Pa -> "997".
        long hpa = (long)(value / 100); // 1 hPa = 100 Pa
        swprintf(valueBuffer, sizeof(valueBuffer) / sizeof(valueBuffer[0]), L"%ld", hpa);
    } else if (type == QUANTITY_HUMIDITY) {
        // Humidity provided in milli-percent RH; display integer percent by truncation.
        // Example: 53260 mRH -> "53".
        long percent = (long)(value / 1000);
        swprintf(valueBuffer, sizeof(valueBuffer) / sizeof(valueBuffer[0]), L"%ld", percent);
    } else {
        // Default: just print the integer value
        swprintf(valueBuffer, sizeof(valueBuffer) / sizeof(valueBuffer[0]), L"%ld", (long)value);
    }

    if (valueText) {
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
    // value text content may have been updated via setValue()
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

    // Assign bitmap data pointers once
    if (type == QUANTITY_TEMPERATURE) {
        pictogram->data = (uint8_t (*)[288])&BITMAP48_TEMPERATURE;
    } else if (type == QUANTITY_HUMIDITY) {
        pictogram->data = (uint8_t (*)[288])&BITMAP48_HUMIDITY;
    } else if (type == QUANTITY_CO2) {
        pictogram->data = (uint8_t (*)[288])&BITMAP48_CO2;
    } else if (type == QUANTITY_PRESSURE) {
        pictogram->data = (uint8_t (*)[288])&BITMAP48_PRESSURE;
    } else {
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

    // Adjust positions for 48x48 if you want more space for text; keeping as-is for now.

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

    initialized = true;
}

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
    icon->posX = ax -2;
    icon->posY = ay + SENSOR_WIDGET_PADDING - 10;

    // Paint the icon
    icon->Paint();
}

TimeWidget::TimeWidget()
    : NavigableWidget(), timeText(nullptr)
{
    buffer[0] = L'\0';
    time.month = 0;
    time.day = 0;
    time.hour = 0;
    time.minute = 0;
    time.second = 0;
}

TimeWidget::~TimeWidget()
{
    if (timeText) {
        delete timeText;
        timeText = nullptr;
    }
}

void TimeWidget::setTime(const Time& t)
{
    time = t;
    // Format as HH:MM
    swprintf(buffer, sizeof(buffer) / sizeof(buffer[0]), L"%02u:%02u", (unsigned)time.hour, (unsigned)time.minute);
    if (timeText) {
        timeText->str = buffer;
    }
}

Time TimeWidget::getTime() const
{
    return time;
}

void TimeWidget::update()
{
    if (!area) return;

    if (!timeText) {
        timeText = new Text(L"00:00");
        timeText->textSize = 4; // readable default
    }

    // Update style from area
    timeText->backgroundColor = area->backgroundColor;
    timeText->color = area->color;

    // Ensure string is set
    const wchar_t* str = (buffer[0] != L'\0') ? buffer : L"--:--";
    timeText->str = str;

    // Center the text within the widget area using 6x8 base font metrics
    size_t len = 0;
    while (str[len] != L'\0') ++len;
    const uint16_t charW = 6 * timeText->textSize;
    const uint16_t charH = 8 * timeText->textSize;
    const uint16_t pixW = (uint16_t)(len * charW);
    const uint16_t pixH = charH;

    timeText->posX = area->posX + (area->sizeX > pixW ? (area->sizeX - pixW) / 2 : 0);
    timeText->posY = area->posY + (area->sizeY > pixH ? (area->sizeY - pixH) / 2 : 0) + 2;

    // Paint background (area) and then the time text
    area->Paint();
    timeText->Paint();
}

void Widget::update()
{
    // Default update behaviour: repaint own area if present
    if (area) {
        area->Paint();
    }
}
