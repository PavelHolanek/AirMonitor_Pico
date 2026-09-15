// TimePickerWiget.cpp - renders "DD.MM. HH:MM" and edits one field at a time
#include "TimePickerWiget.h"
#include "Parameters.h"
#include "Libraries/pico-displayDrivs/gfx/gfx.h"
#include "Clock.h"
#include <wchar.h>
TimePickerWiget::TimePickerWiget()
    : NavigableWidget(), text(nullptr)
{
    hint = nullptr;
    hintPos = FIELD_HOUR;
}

// Column of the first digit of each field within "DD.MM. HH:MM"
static const int fieldColumn[] = { 0, 3, 7, 10 };
static const int timeStringLength = 12; // characters in "DD.MM. HH:MM"

TimePickerWiget::~TimePickerWiget()
{
    if (text)
    {
        delete text;
        text = nullptr;
    }
}

void TimePickerWiget::selected()
{
    if (area)
    {
        area->backgroundColor = PARAM_COLOR_GRAY_1;
        area->color = PARAM_COLOR_BLACK;
    }
    update();
}

void TimePickerWiget::deselected()
{
    if (area)
    {
        area->backgroundColor = PARAM_COLOR_BLACK;
        area->color = PARAM_COLOR_WHITE;
    }
    update();
}

void TimePickerWiget::update()
{
    if (!area) return;

    if (!text)
    {
        text = new Text(L"01.01. 00:00");
        text->textSize = 4;
    }
    if (!hint)
    {
        hint = new Text(L" ");
        hint->textSize = 4;
    }

    // Display currently selected date and time (set by the window on entry)
    swprintf(buffer, sizeof(buffer) / sizeof(buffer[0]), L"%02u.%02u. %02u:%02u",
             (unsigned)selectedTime.day, (unsigned)selectedTime.month,
             (unsigned)selectedTime.hour, (unsigned)selectedTime.minute);
    text->str = buffer;

    // Style sync
    text->backgroundColor = area->backgroundColor;
    text->color = area->color;
    hint->backgroundColor = area->backgroundColor;
    hint->color = area->color;

    // Center text in the area
    const wchar_t* str = text->str;
    size_t len = 0; while (str[len] != L'\0') ++len;
    const uint16_t charW = 6 * text->textSize;
    const uint16_t charH = 8 * text->textSize;
    const uint16_t pixW = (uint16_t)(len * charW);
    const uint16_t pixH = charH;
    const uint16_t textX = area->posX + (area->sizeX > pixW ? (area->sizeX - pixW) / 2 : 0);
    const uint16_t textY = area->posY + (area->sizeY > pixH ? (area->sizeY - pixH) / 2 : 0);
    text->posX = textX;
    text->posY = textY;

    // Position hint directly under the main time text with a small vertical gap
    const uint16_t gap = 4;
    const uint16_t hintCharH = 8 * hint->textSize;
    // Rebuild hint string based on hintPos: underline both digits of the active field
    for (int i = 0; i < timeStringLength; ++i) hintBuffer[i] = L' ';
    const int column = fieldColumn[hintPos];
    hintBuffer[column] = L'^';
    hintBuffer[column + 1] = L'^';
    hintBuffer[timeStringLength] = L'\0';
    hint->str = hintBuffer;

    hint->posX = textX; // aligned to time text
    hint->posY = textY + pixH + gap;

    // Paint background and text
    // Only tear down a buffer we opened ourselves - if one is already up we are
    // drawing into somebody else's, and they will flush it.
    const bool ownsFramebuf = GFX_createFramebuf(area->posX, area->posY, area->sizeX, area->sizeY);
    GFX_fillRect(area->posX, area->posY, area->sizeX, area->sizeY, area->backgroundColor);
    text->Paint();
    hint->Paint();
    if (ownsFramebuf)
    {
        GFX_flush();
        GFX_destroyFramebuf();
    }
}

void TimePickerWiget::moveLeft()
{
    if (hintPos > 0) hintPos--; else hintPos = FIELD_COUNT - 1; // wrap around
    update();
}

void TimePickerWiget::moveRight()
{
    if (hintPos < FIELD_COUNT - 1) hintPos++; else hintPos = 0; // wrap around
    update();
}

// Keep the day inside the selected month, e.g. 31.01. -> 28.02. when stepping the month
void TimePickerWiget::clampDayToMonth()
{
    const uint8_t maxDay = daysInMonthOf(selectedTime.month);
    if (selectedTime.day < 1U) selectedTime.day = 1U;
    if (selectedTime.day > maxDay) selectedTime.day = maxDay;
}

void TimePickerWiget::onPressed()
{
    // no-op for now; behavior moved to moveUp/moveDown
}

void TimePickerWiget::setSelectedTime(const Time& t)
{
    selectedTime = t;
    // A failed RTC read yields month 0 / day 0 - fall back to a valid date
    if (selectedTime.month < 1U || selectedTime.month > 12U) selectedTime.month = 1U;
    clampDayToMonth();
    update();
}

Time TimePickerWiget::getSelectedTime() const
{
    return selectedTime;
}

void TimePickerWiget::moveUp()
{
    switch (hintPos)
    {
        case FIELD_DAY:
        {
            // increment day 1..days in the selected month
            const uint8_t maxDay = daysInMonthOf(selectedTime.month);
            selectedTime.day = (uint8_t)((selectedTime.day >= maxDay) ? 1U : (selectedTime.day + 1U));
            break;
        }
        case FIELD_MONTH:
            // increment month 1..12
            selectedTime.month = (uint8_t)((selectedTime.month >= 12U) ? 1U : (selectedTime.month + 1U));
            clampDayToMonth();
            break;
        case FIELD_HOUR:
            // increment hours 0..23
            selectedTime.hour = (uint8_t)((selectedTime.hour + 1U) % 24U);
            break;
        default: // FIELD_MINUTE
            // increment minutes 0..59
            selectedTime.minute = (uint8_t)((selectedTime.minute + 1U) % 60U);
            break;
    }
    update();
}

void TimePickerWiget::moveDown()
{
    switch (hintPos)
    {
        case FIELD_DAY:
        {
            // decrement day with wrap to the last day of the selected month
            const uint8_t maxDay = daysInMonthOf(selectedTime.month);
            selectedTime.day = (uint8_t)((selectedTime.day <= 1U) ? maxDay : (selectedTime.day - 1U));
            break;
        }
        case FIELD_MONTH:
            // decrement month with wrap underflow
            selectedTime.month = (uint8_t)((selectedTime.month <= 1U) ? 12U : (selectedTime.month - 1U));
            clampDayToMonth();
            break;
        case FIELD_HOUR:
        {
            int h = (int)selectedTime.hour - 1;
            if (h < 0) h = 23;
            selectedTime.hour = (uint8_t)h;
            break;
        }
        default: // FIELD_MINUTE
        {
            int m = (int)selectedTime.minute - 1;
            if (m < 0) m = 59;
            selectedTime.minute = (uint8_t)m;
            break;
        }
    }
    update();
}
