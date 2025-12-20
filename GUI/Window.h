#ifndef WINDOW_H
#define WINDOW_H

#include "Widget.h"

#define SETTINGS_ROWS_COUNT 5

class Window
{
public:
    Window();
    virtual ~Window();

    virtual void joystickAction(JoystickState state) = 0;
};

class MainWindow : public Window
{
public:
    MainWindow();
    virtual ~MainWindow();

    void buttonPressed();
    void joystickMoved(uint8_t direction);
    SensorWidget* getWidgetByType(QUANTITY type);

    NavigableWidget* currentWidget;

    SensorWidget* temperatureWidget;
    SensorWidget* humidityWidget;
    SensorWidget* pressureWidget;
    SensorWidget* co2Widget;
    TimeWidget* timeWidget;
    SettingsWidget* settingWidget;

    void enterQuantityWindow(QUANTITY);
    void enterSettingWindow();

    void joystickAction(JoystickState state) override;
};

class SettingWindow : public Window
{
public:
    SettingWindow();
    virtual ~SettingWindow() {}

    void enterMainWindow();

    NavigableWidget settingsRows[SETTINGS_ROWS_COUNT];
};

#endif // WINDOW_H
