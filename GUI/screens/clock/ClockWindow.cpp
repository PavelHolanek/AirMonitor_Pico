// ClockWindow.cpp - minimal outline implementation
#include "ClockWindow.h"
#include "Parameters.h"
#include "Libraries/pico-displayDrivs/gfx/gfx.h"
#include "Clock.h"
#include "GUIManager.h"

ClockWindow::ClockWindow()
    : Window(), valueRepeat(this, CLOCK_REPEAT_DELAY_MS, CLOCK_REPEAT_PERIOD_MS)
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

    // Any change ends the repeat, the centre included.
    valueRepeat.stop();

    switch (event.direction)
    {
        case JOYSTICK_RIGHT: // wraps, only four fields - fields do not repeat
            picker->moveRight();
            break;
        case JOYSTICK_LEFT:
            picker->moveLeft();
            break;
        case JOYSTICK_UP:
        case JOYSTICK_DOWN:
            stepValue(event.direction);
            valueRepeat.start(event.direction);
            break;
        default: // JOYSTICK_NONE - back in the centre
            break;
    }
}

void ClockWindow::stepValue(JOYSTICK_DIRECTION direction)
{
    if (direction == JOYSTICK_UP)
    {
        picker->moveUp();
    }
    else
    {
        picker->moveDown();
    }
}

void ClockWindow::timerExpired(GuiTimer* timer)
{
    const JOYSTICK_DIRECTION held = valueRepeat.expired(timer);
    if (held != JOYSTICK_NONE && picker)
    {
        stepValue(held);
    }
}
