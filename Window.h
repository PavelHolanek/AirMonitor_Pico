#ifndef WINDOW_H
#define WINDOW_H

#include "Widget.h"

#define SETTINGS_ROWS_COUNT 5

class Window
{
public:
    Window();
    virtual ~Window();
};

class MainWindow
{
public:
    MainWindow();
    virtual ~MainWindow();

    void buttonPressed();
    void joystickMoved(uint8_t direction);
    NavigableWidget* currentWidget;

    Widget* temperatureWidget;
    Widget* humidityWidget;
    Widget* pressureWidget;
    Widget* co2Widget;
    Widget* timeWidget;
    Widget* settingWidget;

    void enterQuantityWindow(QUANTITY);
    void enterSettingWindow();
};

class SettingWindow
{
public:
    SettingWindow();
    virtual ~SettingWindow() {}

    void enterMainWindow();

    NavigableWidget settingsRows[SETTINGS_ROWS_COUNT];
};

#endif // WINDOW_H
