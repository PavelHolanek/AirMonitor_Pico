// SettingWindow.h - settings screen window
#ifndef SETTING_WINDOW_H
#define SETTING_WINDOW_H

#include <stdint.h>
#include "core/Window.h"
#include "SettingWidget.h"

// Capacity of the list, not what fits on screen at once - that is
// SETTINGS_VISIBLE_ROWS_COUNT, and anything past it scrolls.
#define SETTINGS_ROWS_COUNT 12

class SettingWindow : public Window
{
public:
    SettingWindow();
    virtual ~SettingWindow();

    void joystickAction(JoystickState state) override;
    void enterWindow() override;

private:
    void addRow(SettingWidget* row);

    // Repaints everything below the heading. Needed after a scroll, because
    // every visible row has moved to a different slot.
    void paintRows();

    SettingWidget* settingsRows[SETTINGS_ROWS_COUNT];
    uint8_t rowCount;

    // Index into settingsRows, not a slot on screen.
    uint8_t currentRow;

    // Index of the row drawn in the topmost slot.
    uint8_t firstVisibleRow;
};

#endif // SETTING_WINDOW_H
