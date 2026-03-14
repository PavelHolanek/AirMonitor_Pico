// TimePickerWiget.cpp - initial placeholder rendering "00::00"
#include "TimePickerWiget.h"
#include "Parameters.h"
#include "Libraries/pico-displayDrivs/gfx/gfx.h"
#include "Clock.h"
#include <wchar.h>
TimePickerWiget::TimePickerWiget()
    : NavigableWidget(), text(nullptr)
{
    hint = nullptr;
    hintPos = 0;
}

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
        text = new Text(L"00:00");
        text->textSize = 4;
    }
    if (!hint)
    {
        hint = new Text(L"^    ");
        hint->textSize = 4;
    }

    // Display currently selected time (set by the window on entry)
    swprintf(buffer, sizeof(buffer) / sizeof(buffer[0]), L"%02u:%02u", (unsigned)selectedTime.hour, (unsigned)selectedTime.minute);
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
    // Rebuild hint string based on hintPos: two positions only: Hours (0) or Minutes (1)
    // Construct a 5-char preview (H H : M M); place '*' at column 0 for hours, 3 for minutes
    for (int i = 0; i < 5; ++i) hintBuffer[i] = L' ';
    int column = (hintPos == 0) ? 0 : 3; // 0 => under first hour digit, 1 => under first minute digit
    hintBuffer[column] = L'^';
    hintBuffer[5] = L' ';
    hintBuffer[6] = L' ';
    hintBuffer[7] = L'\0';
    hint->str = hintBuffer;

    hint->posX = textX; // aligned to time text
    hint->posY = textY + pixH + gap;

    // Paint background and text
    GFX_createFramebuf(area->posX, area->posY, area->sizeX, area->sizeY);
    GFX_fillRect(area->posX, area->posY, area->sizeX, area->sizeY, area->backgroundColor);
    text->Paint();
    hint->Paint();
    GFX_flush();
    GFX_destroyFramebuf();
}

void TimePickerWiget::moveLeft()
{
    if (hintPos > 0) hintPos--; else hintPos = 1; // wrap around within 0..1
    update();
}

void TimePickerWiget::moveRight()
{
    if (hintPos < 1) hintPos++; else hintPos = 0; // wrap around within 0..1
    update();
}

void TimePickerWiget::onPressed()
{
    // no-op for now; behavior moved to moveUp/moveDown
}

void TimePickerWiget::setSelectedTime(const Time& t)
{
    selectedTime = t;
    update();
}

Time TimePickerWiget::getSelectedTime() const
{
    return selectedTime;
}

void TimePickerWiget::moveUp()
{
    if (hintPos == 0)
    {
        // increment hours 0..23
        unsigned h = (unsigned)selectedTime.hour;
        h = (h + 1) % 24;
        selectedTime.hour = (uint8_t)h;
    }
    else
    {
        // increment minutes 0..59
        unsigned m = (unsigned)selectedTime.minute;
        m = (m + 1) % 60;
        selectedTime.minute = (uint8_t)m;
    }
    update();
}

void TimePickerWiget::moveDown()
{
    if (hintPos == 0)
    {
        // decrement hours with wrap underflow
        int h = (int)selectedTime.hour;
        h = (h - 1);
        if (h < 0) h = 23;
        selectedTime.hour = (uint8_t)h;
    }
    else
    {
        int m = (int)selectedTime.minute;
        m = (m - 1);
        if (m < 0) m = 59;
        selectedTime.minute = (uint8_t)m;
    }
    update();
}
