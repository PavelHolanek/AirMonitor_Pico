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

// paintRows() buffers a slot as one full width strip, and enterWindow() does the
// same with the heading. Both have to fit.
static_assert(PARAM_SCREEN_WIDTH * SETTINGS_ROW_PITCH * 3 <= BUFFER_MAX_SIZE,
              "a settings slot no longer fits the framebuffer");
static_assert(PARAM_SCREEN_WIDTH * SETTINGS_FIRST_ROW_Y * 3 <= BUFFER_MAX_SIZE,
              "the settings heading no longer fits the framebuffer");

SettingWindow::SettingWindow()
    : Window(), settingsRows{nullptr}, rowCount(0U), currentRow(0U), firstVisibleRow(0U),
      backRow(nullptr), saveOnLeave(false)
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
    //addRow(new BoolSettingWidget(L"Zhasnout v klidu", &screenOffWhenIdle));
    addRow(new BoolSettingWidget(L"Vývojářský režim", &developerMode));

    addRow(new ScreenSettingWidget(L"Kalibrace", calibrationWindow));

    // Leaving the screen is a row of its own, so the button stays free for the
    // rows and left/right stay free for enum values.
    // mainWindow must already exist - gui_init() builds it before this window.
    backRow = addRow(new ScreenSettingWidget(L"Zpět", mainWindow));
}

SettingWindow::~SettingWindow()
{
    for (uint8_t i = 0U; i < rowCount; ++i)
    {
        delete settingsRows[i];
        settingsRows[i] = nullptr;
    }
}

SettingWidget* SettingWindow::addRow(SettingWidget* row)
{
    if (!row || rowCount >= SETTINGS_ROWS_COUNT)
    {
        delete row;
        return nullptr;
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
    return row;
}

void SettingWindow::applyAllSettings()
{
    for (uint8_t i = 0U; i < rowCount; ++i)
    {
        settingsRows[i]->applySetting();
    }
}

void SettingWindow::leaveWindow()
{
    applyAllSettings();

    if (saveOnLeave)
    {
        saveOnLeave = false;
        settings_save();
    }
}

void SettingWindow::paintRows()
{
    // One slot at a time, each through its own framebuffer. The strip is the
    // full screen width and a whole pitch tall, so the wipe, the row and the
    // gap below it all land in the same blit - the rows used to be wiped once
    // and then painted over, and every glyph and rounded corner went to the
    // panel a pixel at a time.
    for (uint8_t slot = 0U; slot < SETTINGS_VISIBLE_ROWS_COUNT; ++slot)
    {
        const uint16_t slotY = (uint16_t)(SETTINGS_FIRST_ROW_Y + slot * SETTINGS_ROW_PITCH);
        const uint8_t index = (uint8_t)(firstVisibleRow + slot);

        const bool ownsFramebuf = GFX_createFramebuf(0, slotY,
                                                     PARAM_SCREEN_WIDTH, SETTINGS_ROW_PITCH);
        if (ownsFramebuf)
        {
            // After a scroll the rows sit in different slots, so a slot that
            // lost its row has to come out empty.
            GFX_clearFramebuf(PARAM_COLOR_BLACK);
        }
        else
        {
            // The strip did not fit - painting still works, just straight to
            // the panel, so the wipe has to go there too.
            GFX_fillRect(0, slotY, PARAM_SCREEN_WIDTH, SETTINGS_ROW_PITCH, PARAM_COLOR_BLACK);
        }

        if (index < rowCount)
        {
            SettingWidget* row = settingsRows[index];

            // posY is assigned here: it depends on which slot the row currently
            // occupies, and that changes as the list scrolls.
            row->area->posY = slotY;

            // Both of these repaint the row; which one runs decides whether it
            // comes out inverted. They join the buffer opened above.
            if (index == currentRow)
            {
                row->selected();
            }
            else
            {
                row->deselected();
            }
        }

        if (ownsFramebuf)
        {
            GFX_flush();
            GFX_destroyFramebuf();
        }
    }

    // The slots divide the area below the heading exactly at the current
    // constants, so this folds away - but it keeps the bottom clean if the
    // pitch or the list start ever stops dividing it.
    if (SETTINGS_ROWS_AREA_END < PARAM_SCREEN_HEIGHT)
    {
        GFX_fillRect(0,
                     SETTINGS_ROWS_AREA_END,
                     PARAM_SCREEN_WIDTH,
                     PARAM_SCREEN_HEIGHT - SETTINGS_ROWS_AREA_END,
                     PARAM_COLOR_BLACK);
    }
}

void SettingWindow::enterWindow()
{
    // Only the heading is wiped here - paintRows() covers everything from
    // SETTINGS_FIRST_ROW_Y down, so clearing the whole screen first would blit
    // most of the panel twice.
    const bool ownsFramebuf = GFX_createFramebuf(0, 0, PARAM_SCREEN_WIDTH, SETTINGS_FIRST_ROW_Y);
    if (ownsFramebuf)
    {
        GFX_clearFramebuf(PARAM_COLOR_BLACK);
    }
    else
    {
        GFX_fillRect(0, 0, PARAM_SCREEN_WIDTH, SETTINGS_FIRST_ROW_Y, PARAM_COLOR_BLACK);
    }

    Text title(L"Nastavení", SETTINGS_MARGIN, 16);
    title.textSize = SETTINGS_TITLE_TEXT_SIZE;
    title.color = PARAM_COLOR_WHITE;
    title.backgroundColor = PARAM_COLOR_BLACK;
    title.Paint();

    if (ownsFramebuf)
    {
        GFX_flush();
        GFX_destroyFramebuf();
    }

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
        // Set before the press, because a screen row leaves from inside
        // buttonPressed() and leaveWindow() runs while we are still in here.
        saveOnLeave = (row == backRow);
        row->buttonPressed();
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
