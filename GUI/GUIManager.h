#ifndef GUI_MANAGER_H
#define GUI_MANAGER_H

#include "Base.h"
#include "Window.h"
#include "MainWindow.h"
#include "SettingWindow.h"
#include "ClockWindow.h"
#include "GraphWindow.h"
#include "GraphData.h"

// Forward declarations to avoid including heavy headers here
class MainWindow;
class SettingWindow;

extern Window* currentWindow;
extern MainWindow* mainWindow;
extern SettingWindow* settingsWindow;
extern ClockWindow* clockWindow;
extern GraphWindow* graphWindow;

extern "C"
{
void gui_init();

void gui_dataChanged();

void gui_timeChanged(Time CurerntTime);

void gui_joystick(JoystickState state);

void gui_changeWindow(Window* window);

void gui_idleTimePassed();
}

#endif // GUI_MANAGER_H
