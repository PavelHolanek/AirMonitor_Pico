#ifndef GUI_MANAGER_H
#define GUI_MANAGER_H

#include "Base.h"
#include "Window.h"
#include "MainWindow.h"
#include "SettingWindow.h"
#include "ClockWindow.h"

// Forward declarations to avoid including heavy headers here
class MainWindow;
class SettingWindow;

extern Window* currentWindow;
extern MainWindow* mainWindow;
extern SettingWindow* settingsWindow;
extern ClockWindow* clockWindow;

extern "C"
{
void gui_init();

void gui_dataChanged(QUANTITY type, int32_t value);

void gui_timeChanged(Time CurerntTime);

void gui_joystick(JoystickState state);
// Switch active window. Pass pointer to a concrete Window instance.
void gui_changeWindow(Window* window);
//void gui_showErrorMessage(const char* message);
}

#endif // GUI_MANAGER_H
