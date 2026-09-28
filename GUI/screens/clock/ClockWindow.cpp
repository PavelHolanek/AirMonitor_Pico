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
        picker->area = new Area(PARAM_SCREEN_WIDTH / 2 - 160, PARAM_SCREEN_HEIGHT / 2 - 56, 320, 112);
        picker->area->backgroundColor = PARAM_COLOR_BLACK;
        picker->area->color = PARAM_COLOR_WHITE;
    }
}

void ClockWindow::enterWindow()
{ 
    GFX_fillScreen(PARAM_COLOR_BLACK);
    if (picker) {
        // Initialize the selected time on entry
        Time now = getClockTime();
        picker->setSelectedTime(now);
    }
    if (picker) picker->update();
}

void ClockWindow::joystickEvent(const JoystickEvent& event)
{
    // Left/right move the asterisk within the TimePickerWiget when selected
    if (!picker) return;
    if (event.type == JOYSTICK_EVENT_BUTTON_DOWN)
    {
        // Commit selected time to RTC
        setClockTime(picker->getSelectedTime());
        gui_changeWindow(mainWindow);
        return;
    }
    if (event.type != JOYSTICK_EVENT_DIRECTION) return;
    switch (event.direction)
    {
        case JOYSTICK_RIGHT:
            picker->moveRight();
            break;
        case JOYSTICK_UP:
            picker->moveUp();
            break;
        case JOYSTICK_LEFT:
            picker->moveLeft();
            break;
        case JOYSTICK_DOWN:
            picker->moveDown();
            break;
        default: // JOYSTICK_NONE - back in the centre
            break;
    }
}
