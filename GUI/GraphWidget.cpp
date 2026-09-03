#include "GraphWidget.h"
#include "Parameters.h"
#include "GraphicElement.h"
#include "Libraries/pico-displayDrivs/gfx/gfx.h"
#include "dataManager.h"
#include "Settings.h"
#include <stdio.h>
#include <string.h>





constexpr uint16_t GRAPH_MARGIN = 10U;

constexpr uint16_t GRAPH_INTERVAL_WIDTH = 20U;

constexpr uint16_t GRAPH_LABEL_TEXT_SIZE = 1U;
constexpr uint16_t GRAPH_LABEL_CHAR_WIDTH = 6U * GRAPH_LABEL_TEXT_SIZE;
constexpr uint16_t GRAPH_LABEL_CHAR_HEIGHT = 8U * GRAPH_LABEL_TEXT_SIZE;

constexpr uint16_t GRAPH_LABEL_GAP = 6U;

// Samples handed over to the algorithm. File scope on purpose: a copy of the
// whole dataManager buffer does not belong on a task stack.
graph_sample_t sampleBuffer[DATA_MANAGER_BUFFER_CAPACITY];

struct PixelPoint
{
    int16_t x;
    int16_t y;
    bool valid;
};

// Copies every stored sample of one quantity, oldest first. Samples outside the
// visible frame are kept - the algorithm needs them for the edge points.
size_t collectSamples(QUANTITY quantity)
{
    const size_t available = dataManager_count();
    size_t count = 0U;

    for (size_t i = 0U; i < available && count < DATA_MANAGER_BUFFER_CAPACITY; ++i)
    {
        data_manager_processed_sample_t* sample = dataManager_get_data(i);
        if (sample == nullptr)
        {
            continue;
        }

        sampleBuffer[count].time = sample->time;
        sampleBuffer[count].value = extract_data_for_quantity(sample, quantity);
        ++count;
    }

    return count;
}

int32_t quantityScaleStep(QUANTITY quantity)
{
    switch (quantity)
    {
        case QUANTITY_TEMPERATURE: return 100;    //  1 degree
        case QUANTITY_HUMIDITY: return 1000;      //  1 %
        case QUANTITY_PRESSURE: return 1000;      // 10 hPa
        case QUANTITY_CO2: return 100;            // 100 ppm
        default: return 1;
    }
}

int32_t floorToStep(int32_t value, int32_t step)
{
    int32_t quotient = value / step;
    if ((value % step != 0) && (value < 0))
    {
        quotient--;
    }
    return quotient * step;
}

int32_t ceilToStep(int32_t value, int32_t step)
{
    int32_t quotient = value / step;
    if ((value % step != 0) && (value > 0))
    {
        quotient++;
    }
    return quotient * step;
}

int32_t quantityDisplayDivisor(QUANTITY quantity)
{
    switch (quantity)
    {
        case QUANTITY_TEMPERATURE: return 100;    // 0.01 C -> C
        case QUANTITY_HUMIDITY: return 1000;      // 0.001 % -> %
        case QUANTITY_PRESSURE: return 100;       // Pa -> hPa
        case QUANTITY_CO2: return 1;              // ppm
        default: return 1;
    }
}

void drawAxisLabel(int32_t value, QUANTITY quantity, int16_t axisX, int16_t minX, int16_t y, Color color, Color background)
{
    char text[12];
    snprintf(text, sizeof(text), "%ld", (long)(value / quantityDisplayDivisor(quantity)));

    const int16_t width = (int16_t)(strlen(text) * GRAPH_LABEL_CHAR_WIDTH);

    int16_t x = axisX - (int16_t)GRAPH_LABEL_GAP - width;
    if (x < minX)
    {
        x = minX;
    }

    GFX_setCursor(x, y - (int16_t)(GRAPH_LABEL_CHAR_HEIGHT / 2U));
    GFX_setTextColor(color);
    GFX_setTextBack(background);
    GFX_printf(GRAPH_LABEL_TEXT_SIZE, "%s", text);
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

// Filled area plus the 2 px line between two neighbouring points.
void drawSegment(const PixelPoint& from, const PixelPoint& to, int16_t yBase, Color lineColor, Color areaColor)
{
    const int16_t x0 = from.x;
    const int16_t y0 = from.y;
    const int16_t x1 = to.x;
    const int16_t y1 = to.y;

    if (x0 == x1)
    {
        // Vertical segment: just fill the column down to the baseline.
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

        // Rectangle part of the trapezoid under the segment.
        if (width > 0 && rectH > 0)
        {
            GFX_fillRect(leftX, lowY, width, rectH, areaColor);
        }

        // Triangle part for the sloped difference between the endpoints.
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

GraphWidget::GraphWidget()
    : Widget(),
      quantity(QUANTITY_TEMPERATURE),
      points{}
{
}

GraphWidget::~GraphWidget()
{
}

void GraphWidget::setQuantity(QUANTITY q)
{
    quantity = q;
}

void GraphWidget::setCurrentTime(Time time)
{
    currentTime = time;
    hasCurrentTime = true;
}

bool GraphWidget::computeValueRange(int32_t* outBottom, int32_t* outTop) const
{
    if (!outBottom || !outTop) return false;

    bool found = false;
    int32_t minValue = 0;
    int32_t maxValue = 0;

    for (size_t i = 0U; i < GRAPH_POINTS_COUNT; ++i)
    {
        if (!points.valid[i]) continue;

        if (!found)
        {
            minValue = points.values[i];
            maxValue = points.values[i];
            found = true;
            continue;
        }

        if (points.values[i] < minValue) minValue = points.values[i];
        if (points.values[i] > maxValue) maxValue = points.values[i];
    }

    if (!found) return false;

    const int32_t step = quantityScaleStep(quantity);
    int32_t bottomValue = floorToStep(minValue, step);
    int32_t topValue = ceilToStep(maxValue, step);

    if (topValue <= bottomValue)
    {
        topValue = bottomValue + step;
    }

    *outBottom = bottomValue;
    *outTop = topValue;
    return true;
}

bool GraphWidget::buildInput(graph_input_t* input)
{
    if (input == nullptr || !hasCurrentTime)
    {
        return false;
    }

    const size_t count = collectSamples(quantity);

    if (useRecentData)
    {
        return graph_makeRecentInput(scope, currentTime, sampleBuffer, count, input);
    }
    return graph_makeInputEndingAt(scope, currentTime, timeTo, sampleBuffer, count, input);
}

void GraphWidget::update()
{
    if (!area) return;

    area->Paint();

    const uint16_t right = area->posX + area->sizeX - GRAPH_MARGIN;
    const uint16_t top = area->posY + GRAPH_MARGIN;
    const uint16_t bottom = area->posY + area->sizeY - GRAPH_MARGIN;

    const uint16_t plotWidth = GRAPH_INTERVALS_COUNT * GRAPH_INTERVAL_WIDTH;
    const uint16_t left = right - plotWidth;

    GFX_drawLine(left, top, left, bottom, PARAM_COLOR_WHITE);
    GFX_drawLine(left, bottom, right, bottom, PARAM_COLOR_WHITE);

    graph_input_t input;
    if (!buildInput(&input))
    {
        return;
    }
    if (!graph_computePoints(&input, graphAlgorithm, &points))
    {
        return;
    }

    // Range follows the data, so it changes with every scope and every update.
    int32_t bottomValue = 0;
    int32_t topValue = 1;
    if (!computeValueRange(&bottomValue, &topValue))
    {
        return;
    }
    const int32_t valueRange = topValue - bottomValue;

    drawAxisLabel(topValue, quantity, (int16_t)left, (int16_t)area->posX, (int16_t)top,
                  PARAM_COLOR_WHITE, area->backgroundColor);
    drawAxisLabel(bottomValue, quantity, (int16_t)left, (int16_t)area->posX, (int16_t)bottom,
                  PARAM_COLOR_WHITE, area->backgroundColor);

    const uint16_t plotHeight = (bottom > top) ? (uint16_t)(bottom - top) : 1U;

    // Grid points are evenly spaced - that is what the fixed interval count buys us.
    PixelPoint pixels[GRAPH_POINTS_COUNT];
    for (size_t i = 0U; i < GRAPH_POINTS_COUNT; ++i)
    {
        pixels[i].valid = points.valid[i];
        if (!pixels[i].valid)
        {
            pixels[i].x = 0;
            pixels[i].y = 0;
            continue;
        }

        const uint32_t xRel = (uint32_t)(((uint64_t)i * plotWidth) / GRAPH_INTERVALS_COUNT);
        const uint32_t yRel = (uint32_t)(((uint64_t)(topValue - points.values[i]) * plotHeight) / (uint32_t)valueRange);

        pixels[i].x = (int16_t)(left + xRel);
        pixels[i].y = (int16_t)(top + yRel);
    }

    const Color lineColor = quantityLineColor(quantity);
    const Color areaColor = quantityAreaColor(quantity);
    const int16_t yBase = (bottom > 0U) ? (int16_t)(bottom - 1U) : 0;

    // An invalid point breaks the curve on purpose - the algorithm marks a gap
    // in the data that way, so it must not be bridged by a segment.
    const PixelPoint* prev = nullptr;
    for (size_t i = 0U; i < GRAPH_POINTS_COUNT; ++i)
    {
        if (!pixels[i].valid)
        {
            prev = nullptr;
            continue;
        }

        if (prev)
        {
            drawSegment(*prev, pixels[i], yBase, lineColor, areaColor);
        }
        else if ((i + 1U >= GRAPH_POINTS_COUNT) || !pixels[i + 1U].valid)
        {
            // Point with no valid neighbour on either side would draw nothing.
            GFX_fillRect(pixels[i].x, pixels[i].y, 2, 2, lineColor);
        }

        prev = &pixels[i];
    }
}
