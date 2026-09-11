// CalibrationWindow.cpp - four offset rows, left/right steps the selected one
#include "CalibrationWindow.h"
#include "GUIManager.h"
#include "Parameters.h"
#include "Settings.h"
#include "Libraries/pico-displayDrivs/gfx/gfx.h"
#include <wchar.h>

// One row of the screen. The offsets live in the sensors' internal units so
// they can be added to a raw sample without rescaling; scale converts those to
// what the user reads, step is one joystick flick and limit caps the range.
struct CalibrationRow
{
    const wchar_t* name;
    int32_t* value;
    int32_t scale;        // internal units per displayed unit
    int32_t step;         // internal units per flick
    int32_t limit;        // the offset never leaves [-limit, +limit]
    bool oneDecimal;      // show tenths of the displayed unit
    const wchar_t* unit;  // a literal % must be doubled, see paintRow()
};

static const CalibrationRow CALIBRATION_ROWS[CALIBRATION_ROWS_COUNT] = {
    // hundredths of a degree internally: 0.1 C steps, +-5.0 C
    { L"Teplota", &temperatureCalibration, 100,   10,   500, true,  L"C"   },
    // thousandths of a percent internally: 1 % steps, +-20 %
    { L"Vlhkost", &humidityCalibration,    1000, 1000, 20000, false, L"%%"  },
    // plain ppm: 10 ppm steps, +-500 ppm
    { L"CO2",     &co2Calibration,         1,     10,   500, false, L"ppm" },
    // Pascals internally: 10 hPa steps, +-100 hPa
    { L"Tlak",    &preassureCalibration,   100, 1000, 10000, false, L"hPa" },
};

static const uint16_t CALIBRATION_HINT_Y =
        SETTINGS_FIRST_ROW_Y + CALIBRATION_ROWS_COUNT * SETTINGS_ROW_PITCH + 8;

// Always signed, so it is obvious the number is a correction and not a reading.
static void formatOffset(const CalibrationRow& row, wchar_t* out, size_t count)
{
    const int32_t v = row.value ? *row.value : 0;
    const wchar_t* sign = (v < 0) ? L"-" : ((v > 0) ? L"+" : L"");
    const int32_t magnitude = (v < 0) ? -v : v;

    if (row.oneDecimal && row.scale >= 10)
    {
        swprintf(out, count, L"%ls%ld,%ld %ls",
                 sign,
                 (long)(magnitude / row.scale),
                 (long)((magnitude / (row.scale / 10)) % 10),
                 row.unit);
    }
    else
    {
        swprintf(out, count, L"%ls%ld %ls", sign, (long)(magnitude / row.scale), row.unit);
    }
}

// Text::Paint() hands the string to GFX_printf() as a format string, so a
// literal percent sign travels through the buffer doubled and collapses to one
// only when it is drawn. Right alignment has to measure what will be on screen.
static uint16_t renderedLength(const wchar_t* s)
{
    uint16_t length = 0U;
    while (*s)
    {
        if ((s[0] == L'%') && (s[1] == L'%'))
        {
            ++s;
        }
        ++s;
        ++length;
    }
    return length;
}

CalibrationWindow::CalibrationWindow()
    : Window(), rowAreas{nullptr}, currentRow(0U)
{
    for (uint8_t i = 0U; i < CALIBRATION_ROWS_COUNT; ++i)
    {
        rowAreas[i] = new Area(SETTINGS_MARGIN,
                               (uint16_t)(SETTINGS_FIRST_ROW_Y + i * SETTINGS_ROW_PITCH),
                               SETTINGS_ROW_WIDTH,
                               SETTINGS_ROW_HEIGHT);
        rowAreas[i]->backgroundColor = PARAM_COLOR_BLACK;
        rowAreas[i]->color = PARAM_COLOR_WHITE;
    }
}

CalibrationWindow::~CalibrationWindow()
{
    for (uint8_t i = 0U; i < CALIBRATION_ROWS_COUNT; ++i)
    {
        delete rowAreas[i];
        rowAreas[i] = nullptr;
    }
}

void CalibrationWindow::paintRow(uint8_t index)
{
    if (index >= CALIBRATION_ROWS_COUNT)
    {
        return;
    }

    Area* area = rowAreas[index];
    if (!area)
    {
        return;
    }

    // Same inverse highlight as a settings row: black on white when selected.
    const bool isSelected = (index == currentRow);
    area->backgroundColor = isSelected ? PARAM_COLOR_WHITE : PARAM_COLOR_BLACK;
    area->color = isSelected ? PARAM_COLOR_BLACK : PARAM_COLOR_WHITE;
    area->Paint();

    const CalibrationRow& row = CALIBRATION_ROWS[index];
    const uint16_t textY =
            (uint16_t)(area->posY + (SETTINGS_ROW_HEIGHT - SETTINGS_ROW_TEXT_HEIGHT) / 2);

    Text name(row.name, (uint16_t)(area->posX + SETTINGS_ROW_PADDING), textY);
    name.textSize = SETTINGS_TITLE_TEXT_SIZE;
    name.color = area->color;
    name.backgroundColor = area->backgroundColor;
    name.Paint();

    wchar_t buffer[16];
    formatOffset(row, buffer, sizeof(buffer) / sizeof(buffer[0]));

    Text value(buffer, 0U, textY);
    value.posX = (uint16_t)(area->getEndX() - SETTINGS_ROW_PADDING
                            - renderedLength(buffer) * SETTINGS_ROW_TEXT_WIDTH);
    value.textSize = SETTINGS_TITLE_TEXT_SIZE;
    value.color = area->color;
    value.backgroundColor = area->backgroundColor;
    value.Paint();
}

void CalibrationWindow::enterWindow()
{
    GFX_fillScreen(PARAM_COLOR_BLACK);

    Text title(L"Kalibrace", SETTINGS_MARGIN, 16);
    title.textSize = SETTINGS_TITLE_TEXT_SIZE;
    title.color = PARAM_COLOR_WHITE;
    title.backgroundColor = PARAM_COLOR_BLACK;
    title.Paint();

    // Without this nothing on screen says which way the values move, and the
    // button is the only way out.
    Text hint(L"Vlevo/vpravo mění, stisk zpět", SETTINGS_MARGIN, CALIBRATION_HINT_Y);
    hint.textSize = SETTINGS_HINT_TEXT_SIZE;
    hint.color = PARAM_COLOR_GRAY_1;
    hint.backgroundColor = PARAM_COLOR_BLACK;
    hint.Paint();

    currentRow = 0U;
    for (uint8_t i = 0U; i < CALIBRATION_ROWS_COUNT; ++i)
    {
        paintRow(i);
    }
}

void CalibrationWindow::changeCurrent(int8_t direction)
{
    const CalibrationRow& row = CALIBRATION_ROWS[currentRow];
    if (!row.value)
    {
        return;
    }

    // Clamped rather than wrapped: stepping past +5 C and reappearing at -5 C
    // would silently turn a correction into its opposite.
    int32_t wanted = *row.value + (int32_t)direction * row.step;
    if (wanted > row.limit)
    {
        wanted = row.limit;
    }
    else if (wanted < -row.limit)
    {
        wanted = -row.limit;
    }

    if (wanted != *row.value)
    {
        *row.value = wanted;
        paintRow(currentRow);
    }
}

void CalibrationWindow::moveSelection(uint8_t newRow)
{
    if (newRow == currentRow)
    {
        return;
    }

    const uint8_t previousRow = currentRow;
    currentRow = newRow;
    paintRow(previousRow);
    paintRow(currentRow);
}

void CalibrationWindow::joystickAction(JoystickState state)
{
    if (state.pressed)
    {
        // Leaf screen, so the button leaves - the same as on the graph and clock
        // screens. Left and right are taken by the values themselves.
        gui_changeWindow(settingsWindow);
        return;
    }

    switch (getDominantState(state))
    {
        case 0: // right
            changeCurrent(1);
            break;
        case 2: // left
            changeCurrent(-1);
            break;
        case 1: // up - wraps, only four rows
            moveSelection((currentRow == 0U)
                          ? (uint8_t)(CALIBRATION_ROWS_COUNT - 1U)
                          : (uint8_t)(currentRow - 1U));
            break;
        default: // 3: down
            moveSelection((uint8_t)((currentRow + 1U) % CALIBRATION_ROWS_COUNT));
            break;
    }
}
