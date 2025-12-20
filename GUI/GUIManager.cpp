#include "GUIManager.h"

#include "Window.h"
#include <stdio.h>

Window* currentWindow = nullptr;
MainWindow* mainWindow = nullptr;
SettingWindow* settingsWindows = nullptr;

extern "C" {

void gui_dataChanged(QUANTITY type, int32_t value)
{
    if (currentWindow == reinterpret_cast<Window*>(mainWindow))
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
    currentWindow = reinterpret_cast<Window*>(mainWindow); //implement switchWindow function
}

void gui_timeChanged(Time CurerntTime)
{
    mainWindow->timeWidget->setTime(CurerntTime);
    if (currentWindow == reinterpret_cast<Window*>(mainWindow))
    {
        mainWindow->timeWidget->update();
    }
}

void gui_joystick(JoystickState state)
{
    currentWindow->joystickAction(state);
}

}