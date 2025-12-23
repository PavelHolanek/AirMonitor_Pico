// MainWindow.h - derived window showing main dashboard
#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include "Window.h"

class MainWindow : public Window
{
public:
    MainWindow();
    virtual ~MainWindow();

    void buttonPressed();
    void joystickMoved(uint8_t direction);
    QuantityWidget* getWidgetByType(QUANTITY type);

    NavigableWidget* currentWidget;

    QuantityWidget* temperatureWidget;
    QuantityWidget* humidityWidget;
    QuantityWidget* pressureWidget;
    QuantityWidget* co2Widget;
    TimeWidget* timeWidget;
    SettingsWidget* settingWidget;

    void enterQuantityWindow(QUANTITY);
    void enterSettingWindow();

    void joystickAction(JoystickState state) override;
    void enterWindow() override;
};

#endif // MAIN_WINDOW_H

