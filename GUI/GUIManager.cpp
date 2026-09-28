#include "GUIManager.h"
#include "screens/main/MainWindow.h"
#include "screens/settings/SettingWindow.h"
#include "screens/clock/ClockWindow.h"
#include "screens/graph/GraphWindow.h"
#include "screens/calibration/CalibrationWindow.h"
#include "core/GuiTimer.h"
#include "Settings.h"
#include <stdio.h>

Window* currentWindow = nullptr;
MainWindow* mainWindow = nullptr;
SettingWindow* settingsWindow = nullptr;
ClockWindow* clockWindow = nullptr;
GraphWindow* graphWindow = nullptr;
CalibrationWindow* calibrationWindow = nullptr;

// Drops the highlight on the main screen once the joystick has been left alone.
static void idleTimePassed()
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

// Owned by the manager, not a window, so a window change does not stop it.
class IdleListener : public GuiTimerListener
{
public:
    void timerExpired(GuiTimer* timer) override
    {
        (void)timer;
        idleTimePassed();
    }
};

static IdleListener idleListener;
static GuiTimer idleTimer(&idleListener);

extern "C" {

void gui_dataChanged()
{
    if (currentWindow == mainWindow)
    {
        mainWindow->updateData();
    }
    else if (currentWindow == graphWindow)
    {
        graphWindow->updateData();
    }
}

void gui_init()
{
    printf("gui_init \n");
    mainWindow = new MainWindow{};
    clockWindow = new ClockWindow{};
    graphWindow = new GraphWindow{};
    calibrationWindow = new CalibrationWindow{};
    settingsWindow = new SettingWindow{};
    currentWindow = mainWindow;
    currentWindow->enterWindow();
}

void gui_timeChanged(Time CurerntTime)
{
    mainWindow->timeWidget->setTime(CurerntTime);
    graphWindow->setCurrentTime(CurerntTime);

    if (currentWindow == mainWindow)
    {
        mainWindow->timeWidget->update();
    }
    else if (currentWindow == graphWindow)
    {
        graphWindow->updateData();
    }
}

void gui_handleEvent(const GuiEvent* event)
{
    if (!event)
    {
        return;
    }

    switch (event->type)
    {
        case GUI_EVENT_JOYSTICK:
            idleTimer.start(idleTime);
            currentWindow->joystickEvent(event->joystick);
            break;
    }
}

uint32_t gui_msUntilNextTimer()
{
    return GuiTimer::msUntilNext();
}

void gui_processTimers()
{
    GuiTimer::processExpired();
}

void gui_changeWindow(Window* window)
{
    currentWindow->leaveWindow();
    // A timer of the window just left must never fire into the next one.
    GuiTimer::stopAllOwnedBy(currentWindow);
    currentWindow = window;
    currentWindow->enterWindow();
}

}
