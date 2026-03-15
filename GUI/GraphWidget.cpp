#include "GraphWidget.h"
#include "Parameters.h"
#include "GraphicElement.h"
#include "Libraries/pico-displayDrivs/gfx/gfx.h"

namespace
{
constexpr uint32_t GRAPH_LOOKBACK_SECONDS = 20U * 60U;
const gui_graph_sample_t EMPTY_GRAPH_SAMPLE =
{
    {0, 0, 0, 0, 0},
    0,
    0,
    0,
    0
};

uint32_t timeToTotalSeconds(const Time& t)
{
    static const uint16_t daysBeforeMonth[12] =
    {
        0,   // Jan
        31,  // Feb
        59,  // Mar
        90,  // Apr
        120, // May
        151, // Jun
        181, // Jul
        212, // Aug
        243, // Sep
        273, // Oct
        304, // Nov
        334  // Dec
    };

    uint8_t month = t.month;
    uint8_t day = t.day;
    uint8_t hour = t.hour;
    uint8_t minute = t.minute;
    uint8_t second = t.second;

    if (month < 1U || month > 12U) month = 1U;
    if (day < 1U || day > 31U) day = 1U;
    if (hour > 23U) hour = 23U;
    if (minute > 59U) minute = 59U;
    if (second > 59U) second = 59U;

    const uint32_t days = (uint32_t)daysBeforeMonth[month - 1U] + (uint32_t)(day - 1U);
    return (((days * 24U) + (uint32_t)hour) * 60U + (uint32_t)minute) * 60U + (uint32_t)second;
}

int32_t sampleValueForQuantity(const gui_graph_sample_t& sample, QUANTITY quantity)
{
    switch (quantity)
    {
        case QUANTITY_TEMPERATURE: return sample.temperature_c;
        case QUANTITY_HUMIDITY: return sample.humidity_rh;
        case QUANTITY_PRESSURE: return sample.pressure_pa;
        case QUANTITY_CO2: return sample.co2_ppm;
        default: return 0;
    }
}

Color quantityLineColor(QUANTITY quantity)
{
    switch (quantity)
    {
        case QUANTITY_TEMPERATURE: return COLOR_TEMPERATURE;
        case QUANTITY_HUMIDITY: return COLOR_HUMIDITY;
        case QUANTITY_PRESSURE: return COLOR_PRESSURE;
        case QUANTITY_CO2: return COLOR_CO2;
        default: return PARAM_COLOR_WHITE;
    }
}

Color quantityAreaColor(QUANTITY quantity)
{
    switch (quantity)
    {
        case QUANTITY_TEMPERATURE: return COLOR_TEMPERATURE_2;
        case QUANTITY_HUMIDITY: return COLOR_HUMIDITY_2;
        case QUANTITY_PRESSURE: return COLOR_PRESSURE_2;
        case QUANTITY_CO2: return COLOR_CO2_2;
        default: return PARAM_COLOR_GRAY_1;
    }
}

void drawLine2Px(int16_t x0, int16_t y0, int16_t x1, int16_t y1, Color color)
{
    GFX_drawLine(x0, y0, x1, y1, color);

    const int16_t dx = (x1 >= x0) ? (x1 - x0) : (x0 - x1);
    const int16_t dy = (y1 >= y0) ? (y1 - y0) : (y0 - y1);
    if (dx >= dy)
    {
        GFX_drawLine(x0, y0 + 1, x1, y1 + 1, color);
    }
    else
    {
        GFX_drawLine(x0 + 1, y0, x1 + 1, y1, color);
    }
}
}

GraphWidget::GraphWidget()
    : Widget(),
      quantity(QUANTITY_TEMPERATURE),
      data(&EMPTY_GRAPH_SAMPLE),
      dataCount(0U),
      fromTime{0, 0, 0, 0, 0},
      toTime{0, 0, 0, 0, 0}
{
    for (size_t i = 0; i < GRAPH_WIDGET_MAX_POINTS; ++i)
    {
        coordinates[i].x = 0;
        coordinates[i].y = 0;
        coordinates[i].valid = false;
    }
}

GraphWidget::~GraphWidget()
{
}

void GraphWidget::setQuantity(QUANTITY q)
{
    quantity = q;
}

void GraphWidget::setData(const gui_graph_sample_t* newData, size_t count)
{
    if (newData == nullptr || count == 0U)
    {
        data = &EMPTY_GRAPH_SAMPLE;
        dataCount = 0U;
        return;
    }
    data = newData;
    dataCount = count;
}

void GraphWidget::setTimeFrame(Time from, Time to)
{
    fromTime = from;
    toTime = to;
}

void GraphWidget::getScaleRange(int32_t* outBottom, int32_t* outTop) const
{
    if (!outBottom || !outTop) return;

    switch (quantity)
    {
        case QUANTITY_TEMPERATURE:
            *outBottom = 2000;   // 20.0 C
            *outTop = 3000;       // 30.0 C
            break;
        case QUANTITY_HUMIDITY:
            *outBottom = 0;       // 0 %
            *outTop = 100000;     // 100 %
            break;
        case QUANTITY_PRESSURE:
            *outBottom = 92000;   // 920 hPa
            *outTop = 105000;     // 1050 hPa
            break;
        case QUANTITY_CO2:
            *outBottom = 350;     // 300 ppm
            *outTop = 1800;       // 3000 ppm
            break;
        default:
            *outBottom = 0;
            *outTop = 1;
            break;
    }
}

void GraphWidget::computeCoordinates()
{
    for (size_t i = 0; i < GRAPH_WIDGET_MAX_POINTS; ++i)
    {
        coordinates[i].x = 0U;
        coordinates[i].y = 0U;
        coordinates[i].valid = false;
    }

    if (!area || dataCount == 0U)
    {
        return;
    }

    const uint32_t endSec = timeToTotalSeconds(data[dataCount - 1U].time);
    const uint32_t startSec = (endSec > GRAPH_LOOKBACK_SECONDS) ? (endSec - GRAPH_LOOKBACK_SECONDS) : 0U;
    const uint32_t frameSpanSec = (endSec > startSec) ? (endSec - startSec) : 1U;

    size_t firstIndex = dataCount;
    size_t lastIndex = dataCount;
    for (size_t i = 0; i < dataCount; ++i)
    {
        const uint32_t sec = timeToTotalSeconds(data[i].time);
        if (sec >= startSec && sec <= endSec)
        {
            if (firstIndex == dataCount)
            {
                firstIndex = i;
            }
            lastIndex = i;
        }
    }

    if (firstIndex == dataCount || lastIndex == dataCount || firstIndex > lastIndex)
    {
        return;
    }

    const uint16_t left = area->posX + 10U;
    const uint16_t top = area->posY + 10U;
    const uint16_t plotWidth = (area->sizeX > 20U) ? (area->sizeX - 20U) : 1U;
    const uint16_t plotHeight = (area->sizeY > 20U) ? (area->sizeY - 20U) : 1U;

    int32_t bottomValue = 0;
    int32_t topValue = 1;
    getScaleRange(&bottomValue, &topValue);
    if (topValue <= bottomValue)
    {
        topValue = bottomValue + 1;
    }
    const int32_t valueRange = topValue - bottomValue;

    uint32_t minIntervalSec = frameSpanSec / GRAPH_WIDGET_MAX_POINTS;
    if (minIntervalSec == 0U)
    {
        minIntervalSec = 1U;
    }

    size_t coordIndex = 0U;
    uint32_t lastAcceptedSec = 0U;
    bool hasLastAccepted = false;
    for (size_t i = firstIndex; i <= lastIndex && coordIndex < GRAPH_WIDGET_MAX_POINTS; ++i)
    {
        const uint32_t sec = timeToTotalSeconds(data[i].time);
        if (sec < startSec || sec > endSec)
        {
            continue;
        }

        if (hasLastAccepted && sec < lastAcceptedSec)
        {
            continue;
        }
        if (hasLastAccepted && (sec - lastAcceptedSec) < minIntervalSec)
        {
            continue;
        }
        int32_t value = sampleValueForQuantity(data[i], quantity);
        if (value < bottomValue) value = bottomValue;
        if (value > topValue) value = topValue;

        const uint32_t xRel = (uint32_t)(((uint64_t)(sec - startSec) * plotWidth) / frameSpanSec);
        const uint32_t yRel = (uint32_t)(((uint64_t)(topValue - value) * plotHeight) / (uint32_t)valueRange);

        coordinates[coordIndex].x = (uint16_t)(left + xRel);
        coordinates[coordIndex].y = (uint16_t)(top + yRel);
        coordinates[coordIndex].valid = true;

        lastAcceptedSec = sec;
        hasLastAccepted = true;
        coordIndex++;
    }
}

void GraphWidget::update()
{
    if (!area) return;

    area->Paint();
    computeCoordinates();

    const uint16_t left = area->posX + 10;
    const uint16_t right = area->posX + area->sizeX - 10;
    const uint16_t top = area->posY + 10;
    const uint16_t bottom = area->posY + area->sizeY - 10;
    const Color lineColor = quantityLineColor(quantity);
    const Color areaColor = quantityAreaColor(quantity);

    GFX_drawLine(left, top, left, bottom, PARAM_COLOR_WHITE);
    GFX_drawLine(left, bottom, right, bottom, PARAM_COLOR_WHITE);

    GraphCoordinate* prev = nullptr;
    for (size_t i = 0; i < GRAPH_WIDGET_MAX_POINTS; ++i)
    {
        if (!coordinates[i].valid) continue;
        if (prev)
        {
            const int16_t x0 = (int16_t)prev->x;
            const int16_t y0 = (int16_t)prev->y;
            const int16_t x1 = (int16_t)coordinates[i].x;
            const int16_t y1 = (int16_t)coordinates[i].y;
            const int16_t yBase = (bottom > 0U) ? (int16_t)(bottom - 1U) : 0;

            if (x0 == x1)
            {
                // Vertical segment: just fill the column down to baseline.
                const int16_t yTop = (y0 < y1) ? y0 : y1;
                const int16_t h = (int16_t)(yBase - yTop + 1);
                if (h > 0)
                {
                    GFX_fillRect(x0, yTop, 1, h, areaColor);
                }
            }
            else
            {
                int16_t leftX = x0;
                int16_t leftY = y0;
                int16_t rightX = x1;
                int16_t rightY = y1;

                if (leftX > rightX)
                {
                    leftX = x1;
                    leftY = y1;
                    rightX = x0;
                    rightY = y0;
                }

                const int16_t lowY = (leftY > rightY) ? leftY : rightY;
                const int16_t highY = (leftY < rightY) ? leftY : rightY;
                const int16_t width = (int16_t)(rightX - leftX + 1);
                const int16_t rectH = (int16_t)(yBase - lowY + 1);

                // Rectangle part of trapezoid under the segment.
                if (width > 0 && rectH > 0)
                {
                    GFX_fillRect(leftX, lowY, width, rectH, areaColor);
                }

                // Triangle part for the sloped difference between endpoints.
                if (highY < lowY)
                {
                    if (leftY < rightY)
                    {
                        GFX_fillTriangle(leftX, highY, leftX, lowY, rightX, lowY, areaColor);
                    }
                    else
                    {
                        GFX_fillTriangle(rightX, highY, rightX, lowY, leftX, lowY, areaColor);
                    }
                }
            }

            drawLine2Px(x0, y0, x1, y1, lineColor);
        }
        prev = &coordinates[i];
    }
}
