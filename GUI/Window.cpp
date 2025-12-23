#include "Window.h"

Window::Window()
{
}
Window::~Window() {}

uint8_t Window::getDominantState(JoystickState state)
{
    // Mirror the directional decision logic used in MainWindow::joystickAction
    // Returns: 0=right, 1=up, 2=left, 3=down
    if (state.vertical > state.horizontal && state.vertical > -state.horizontal)
    {
        return 0; // right
    }
    else if (state.vertical < state.horizontal && state.vertical > -state.horizontal)
    {
        return 1; // up
    }
    else if (state.vertical < state.horizontal && state.vertical < -state.horizontal)
    {
        return 2; // left
    }
    else
    {
        return 3; // down
    }
}
