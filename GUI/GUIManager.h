#ifndef GUI_MANAGER_H
#define GUI_MANAGER_H

#include "Base.h"
#include "GuiEvent.h"
#include "core/Window.h"
#include "screens/main/MainWindow.h"
#include "screens/settings/SettingWindow.h"
#include "screens/clock/ClockWindow.h"
#include "screens/graph/GraphWindow.h"
#include "screens/calibration/CalibrationWindow.h"

extern Window* currentWindow;
extern MainWindow* mainWindow;
extern SettingWindow* settingsWindow;
extern ClockWindow* clockWindow;
extern GraphWindow* graphWindow;
extern CalibrationWindow* calibrationWindow;

extern "C"
{
void gui_init();

void gui_dataChanged();

void gui_timeChanged(Time CurerntTime);

void gui_handleEvent(const GuiEvent* event);

uint32_t gui_msUntilNextTimer();

void gui_processTimers();

void gui_changeWindow(Window* window);
}

#endif // GUI_MANAGER_H
