#ifndef WINDOW_H
#define WINDOW_H

#include "Base.h"
#include "Joystick.h"
#include "GuiTimer.h"

class Window : public GuiTimerListener
{
public:
    Window();
    virtual ~Window();

    virtual void joystickEvent(const JoystickEvent& event) = 0;
    virtual void enterWindow() = 0;
    virtual void leaveWindow(){;}

    // Timers owned by a window are stopped when it is left, so this only ever
    // runs on the window on screen.
    void timerExpired(GuiTimer* timer) override { (void)timer; }
};

#endif // WINDOW_H
