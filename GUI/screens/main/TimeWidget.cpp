#include "TimeWidget.h"
#include "Parameters.h"

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
    if (timeText)
    {
        delete timeText;
        timeText = nullptr;
    }
}

void TimeWidget::setTime(const Time& t)
{
    time = t;
    // Format as HH:MM
    swprintf(buffer, sizeof(buffer) / sizeof(buffer[0]), L"%02u:%02u", (unsigned)time.hour, (unsigned)time.minute);
    if (timeText)
    {
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

    if (!timeText)
    {
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

void TimeWidget::selected()
{
    area->backgroundColor = PARAM_COLOR_GRAY_1;
    area->color = PARAM_COLOR_BLACK;
    update();
}
void TimeWidget::deselected()
{
    area->backgroundColor = PARAM_COLOR_BLACK;
    area->color = PARAM_COLOR_WHITE;
    update();
}

