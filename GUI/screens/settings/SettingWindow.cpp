// SettingWindow.cpp - scrolling vertical list of setting rows
#include "SettingWindow.h"
#include "BoolSettingWidget.h"
#include "EnumSettingWidget.h"
#include "ScreenSettingWidget.h"
#include "Parameters.h"
#include "Settings.h"
#include "GUIManager.h"
#include "Libraries/pico-displayDrivs/gfx/gfx.h"

// Labels are indexed by the value of the setting. The doubled const keeps both
// the pointers and the strings in flash, so a table costs no RAM.
static const wchar_t* const GRAPH_ALGORITHM_LABELS[] = {
    L"Lineární",  // GRAPH_ALGORITHM_LINEAR_INTERPOLATION
    L"Průměr",    // GRAPH_ALGORITHM_BUCKET_AVERAGE
};
static_assert(sizeof(GRAPH_ALGORITHM_LABELS) / sizeof(GRAPH_ALGORITHM_LABELS[0])
              == (size_t)GRAPH_ALGORITHM_COUNT,
              "a GRAPH_ALGORITHM value has no label");

static const wchar_t* const INTERVAL_LABELS[] = {
    L"2 min",   // INTERVAL_2_MIN
    L"5 min",   // INTERVAL_5_MIN
    L"10 min",  // INTERVAL_10_MIN
};

// The rows reach the enums through uint8_t*, because arm-none-eabi-gcc defaults
// to -fshort-enums and these are one byte wide. If an enumerator above 255 ever
// widens them, this stops the build instead of letting a row scribble over the
// three bytes that follow.
static_assert(sizeof(graphAlgorithm) == 1U, "GRAPH_ALGORITHM is no longer one byte");
static_assert(sizeof(meassurementInterval) == 1U, "INTERVAL is no longer one byte");

SettingWindow::SettingWindow()
    : Window(), settingsRows{nullptr}, rowCount(0U), currentRow(0U), firstVisibleRow(0U)
{
    // A title has to leave room for its value on the same line: together they
    // must stay under (SETTINGS_ROW_WIDTH - 2 * SETTINGS_ROW_PADDING) /
    // SETTINGS_ROW_TEXT_WIDTH characters, which is 23 at the current text size.
    addRow(new BoolSettingWidget(L"Pevný rozsah grafu", &useFixedValuesSpanForGraph));
    addRow(new EnumSettingWidget(L"Algoritmus", (uint8_t*)&graphAlgorithm,
                                 GRAPH_ALGORITHM_LABELS,
                                 sizeof(GRAPH_ALGORITHM_LABELS) / sizeof(GRAPH_ALGORITHM_LABELS[0])));
    addRow(new EnumSettingWidget(L"Interval měření", (uint8_t*)&meassurementInterval,
                                 INTERVAL_LABELS,
                                 sizeof(INTERVAL_LABELS) / sizeof(INTERVAL_LABELS[0])));
    addRow(new BoolSettingWidget(L"Podsvícení v klidu", &screenBacklightWhenIdle));
    addRow(new BoolSettingWidget(L"Vývojářský režim", &developerMode));

    addRow(new ScreenSettingWidget(L"Kalibrace", calibrationWindow));

    // Leaving the screen is a row of its own, so the button stays free for the
    // rows and left/right stay free for enum values.
    // mainWindow must already exist - gui_init() builds it before this window.
    addRow(new ScreenSettingWidget(L"Zpět", mainWindow));
}

SettingWindow::~SettingWindow()
{
    for (uint8_t i = 0U; i < rowCount; ++i)
    {
        delete settingsRows[i];
        settingsRows[i] = nullptr;
    }
}

void SettingWindow::addRow(SettingWidget* row)
{
    if (!row || rowCount >= SETTINGS_ROWS_COUNT)
    {
        delete row;
        return;
    }

    // posY is assigned by paintRows(): it depends on which slot the row
    // currently occupies, and that changes as the list scrolls.
    row->area = new Area(SETTINGS_MARGIN,
                         SETTINGS_FIRST_ROW_Y,
                         SETTINGS_ROW_WIDTH,
                         SETTINGS_ROW_HEIGHT);
    row->area->backgroundColor = PARAM_COLOR_BLACK;
    row->area->color = PARAM_COLOR_WHITE;

    settingsRows[rowCount] = row;
    rowCount++;
}

void SettingWindow::paintRows()
{
    // Wipe everything below the heading first - after a scroll the rows sit in
    // different slots and the one that left the view has to disappear.
    GFX_fillRect(0,
                 SETTINGS_FIRST_ROW_Y,
                 PARAM_SCREEN_WIDTH,
                 PARAM_SCREEN_HEIGHT - SETTINGS_FIRST_ROW_Y,
                 PARAM_COLOR_BLACK);

    for (uint8_t slot = 0U; slot < SETTINGS_VISIBLE_ROWS_COUNT; ++slot)
    {
        const uint8_t index = (uint8_t)(firstVisibleRow + slot);
        if (index >= rowCount)
        {
            break;
        }

        SettingWidget* row = settingsRows[index];
        row->area->posY = (uint16_t)(SETTINGS_FIRST_ROW_Y + slot * SETTINGS_ROW_PITCH);

        // Both of these repaint the row; which one runs decides whether it
        // comes out inverted.
        if (index == currentRow)
        {
            row->selected();
        }
        else
        {
            row->deselected();
        }
    }
}

void SettingWindow::enterWindow()
{
    GFX_fillScreen(PARAM_COLOR_BLACK);

    Text title(L"Nastavení", SETTINGS_MARGIN, 16);
    title.textSize = SETTINGS_TITLE_TEXT_SIZE;
    title.color = PARAM_COLOR_WHITE;
    title.backgroundColor = PARAM_COLOR_BLACK;
    title.Paint();

    currentRow = 0U;
    firstVisibleRow = 0U;
    paintRows();
}

void SettingWindow::joystickAction(JoystickState state)
{
    if (rowCount == 0U)
    {
        return;
    }

    SettingWidget* row = settingsRows[currentRow];

    if (state.pressed)
    {
        row->applySetting();
        return;
    }

    uint8_t newRow;
    switch (getDominantState(state))
    {
        case 0: // right - the row decides, only enum rows react
            row->moveRight();
            return;
        case 2: // left
            row->moveLeft();
            return;
        case 1: // up - wraps, so the last row is one flick from the first
            newRow = (currentRow == 0U) ? (uint8_t)(rowCount - 1U) : (uint8_t)(currentRow - 1U);
            break;
        default: // 3: down
            newRow = (uint8_t)((currentRow + 1U) % rowCount);
            break;
    }

    if (newRow == currentRow)
    {
        return;
    }

    // Pull the view along so the selection stays inside it. Stepping off either
    // end moves the list by one; wrapping jumps it to the far end.
    const uint8_t previousFirst = firstVisibleRow;
    if (newRow < firstVisibleRow)
    {
        firstVisibleRow = newRow;
    }
    else if (newRow >= (uint8_t)(firstVisibleRow + SETTINGS_VISIBLE_ROWS_COUNT))
    {
        firstVisibleRow = (uint8_t)(newRow - SETTINGS_VISIBLE_ROWS_COUNT + 1U);
    }

    const uint8_t previousRow = currentRow;
    currentRow = newRow;

    if (firstVisibleRow != previousFirst)
    {
        paintRows();
    }
    else
    {
        // Nothing moved, so only the two rows that changed state need redrawing.
        settingsRows[previousRow]->deselected();
        settingsRows[currentRow]->selected();
    }
}
