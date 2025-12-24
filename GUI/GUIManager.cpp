#include "GUIManager.h"
#include "MainWindow.h"
#include "SettingWindow.h"
#include "ClockWindow.h"
#include <stdio.h>

Window* currentWindow = nullptr;
MainWindow* mainWindow = nullptr;
SettingWindow* settingsWindow = nullptr;
ClockWindow* clockWindow = nullptr;

extern "C" {

void gui_dataChanged(QUANTITY type, int32_t value)
{
    if (currentWindow == mainWindow)
    {
        mainWindow->getWidgetByType(type)->setValue(value);
        mainWindow->getWidgetByType(type)->update();
    }
    // else if graph widget
}

void gui_init()
{
    printf("gui_init \n");
    mainWindow = new MainWindow{};
    clockWindow = new ClockWindow{};
    currentWindow = mainWindow;
    currentWindow->enterWindow();
}

void gui_timeChanged(Time CurerntTime)
{
    mainWindow->timeWidget->setTime(CurerntTime);
    if (currentWindow == mainWindow)
    {
        mainWindow->timeWidget->update();
    }
}

void gui_joystick(JoystickState state)
{
    currentWindow->joystickAction(state);
}

void gui_changeWindow(Window* window)
{
    currentWindow->leaveWindow();
    currentWindow = window;
    currentWindow->enterWindow();
}

void gui_idleTimePassed()
{
    if (currentWindow == mainWindow)
    {
        if(mainWindow->currentWidget)
        {
            mainWindow->currentWidget->deselected();
            mainWindow->currentWidget->update();
            mainWindow->currentWidget = nullptr;
        }   
    }
}

}
