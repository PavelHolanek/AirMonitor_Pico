// ClockWindow.h - window dedicated to clock/time display and controls
#ifndef CLOCK_WINDOW_H
#define CLOCK_WINDOW_H

#include "Window.h"
#include "TimePickerWiget.h"

class ClockWindow : public Window
{
public:
    ClockWindow();
    virtual ~ClockWindow() {}

    // Window interface
    void joystickAction(JoystickState state) override;
    void enterWindow() override;

private:
    TimePickerWiget* picker;
};

#endif // CLOCK_WINDOW_H
