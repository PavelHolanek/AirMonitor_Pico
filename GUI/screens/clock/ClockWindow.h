// ClockWindow.h - window dedicated to clock/time display and controls
#ifndef CLOCK_WINDOW_H
#define CLOCK_WINDOW_H

#include "core/Window.h"
#include "core/HoldRepeat.h"
#include "TimePickerWiget.h"

// Holding up or down keeps stepping the selected field: the first repeat after
// the delay, then one every period.
#define CLOCK_REPEAT_DELAY_MS 700U
#define CLOCK_REPEAT_PERIOD_MS 500U

class ClockWindow : public Window
{
public:
    ClockWindow();
    virtual ~ClockWindow() {}

    // Window interface
    void joystickEvent(const JoystickEvent& event) override;
    void timerExpired(GuiTimer* timer) override;
    void enterWindow() override;

private:
    void stepValue(JOYSTICK_DIRECTION direction);

    TimePickerWiget* picker;
    HoldRepeat valueRepeat;
};

#endif // CLOCK_WINDOW_H
