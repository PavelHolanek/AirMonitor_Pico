#ifndef WINDOW_H
#define WINDOW_H

#include "Base.h" // brings JoystickState

class Window
{
public:
    Window();
    virtual ~Window();

    virtual void joystickAction(JoystickState state) = 0;
    virtual void enterWindow() = 0;
    virtual void leaveWindow(){;}

    // Returns dominant direction bucket based on joystick axes comparison.
    // Mapping (0..3): 0=right, 1=up, 2=left, 3=down
    static uint8_t getDominantState(JoystickState state);
};

#endif // WINDOW_H
