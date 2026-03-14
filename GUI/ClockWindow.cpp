// ClockWindow.cpp - minimal outline implementation
#include "ClockWindow.h"
#include "Parameters.h"
#include "Libraries/pico-displayDrivs/gfx/gfx.h"
#include "Clock.h"
#include "GUIManager.h"

ClockWindow::ClockWindow()
{
    picker = new TimePickerWiget();
    if (picker)
    {
        picker->area = new Area(PARAM_SCREEN_WIDTH / 2 - 100, PARAM_SCREEN_HEIGHT / 2 - 30, 200, 60);
        picker->area->backgroundColor = PARAM_COLOR_BLACK;
        picker->area->color = PARAM_COLOR_WHITE;
    }
}

void ClockWindow::enterWindow()
{
    GFX_fillRect(0, 0, PARAM_SCREEN_WIDTH, PARAM_SCREEN_HEIGHT, PARAM_COLOR_BLACK);
    if (picker) {
        // Initialize the selected time on entry
        Time now = getClockTime();
        picker->setSelectedTime(now);
    }
    if (picker) picker->update();
}

void ClockWindow::joystickAction(JoystickState state)
{
    // Left/right move the asterisk within the TimePickerWiget when selected
    if (!picker) return;
    if (state.pressed)
    {
        // Commit selected time to RTC
        setClockTime(picker->getSelectedTime());
        gui_changeWindow(mainWindow);
        return;
    }
    switch (getDominantState(state))
    {
        case 0: // right
            picker->moveRight();
            break;
        case 1: // up
            picker->moveUp();
            break;
        case 2: // left
            picker->moveLeft();
            break;
        default: // 3: down
            picker->moveDown();
            break;
    }
}
