#ifndef GUI_MANAGER_H
#define GUI_MANAGER_H

#include "Base.h"
#include "Window.h"

extern Window* currentWindow;
extern MainWindow* mainWindow;
extern SettingWindow* settingsWindows;

extern "C"
{
void gui_init();

void gui_dataChanged(QUANTITY type, int32_t value);

void gui_timeChanged(Time CurerntTime);
//void gui_joystickMoved();

//void gui_showErrorMessage(const char* message);
}


#endif // GUI_MANAGER_H