#include "Window.h"
#include "Parameters.h"
#include "Libraries/pico-displayDrivs/gfx/gfx.h"

Window::Window()
{
}
Window::~Window() {}



MainWindow::MainWindow()
{
    // paint whole screen with background color
    GFX_fillRect(0, 0, PARAM_SCREEN_WIDTH, PARAM_SCREEN_HEIGHT, PARAM_COLOR_BLACK);
    GFX_flush();

    // create sensor widgets with their QUANTITY types
    temperatureWidget = new SensorWidget(QUANTITY_TEMPERATURE);
    humidityWidget    = new SensorWidget(QUANTITY_HUMIDITY);
    pressureWidget   = new SensorWidget(QUANTITY_PRESSURE);
    co2Widget         = new SensorWidget(QUANTITY_CO2);

    // time and settings are generic navigable widgets
    timeWidget    = new NavigableWidget();
    settingWidget = new SettingsWidget();

    // create simple area layouts for each widget (positions/sizes are examples)
    if (temperatureWidget) {
        temperatureWidget->area = new Area(SENSOR_WIDGET_MARGIN,
                                           HEADER_HEIGHT + SENSOR_WIDGET_MARGIN,
                                           SENSOR_WIDGET_WIDTH,
                                           SENSOR_WIDGET_HEIGHT);
        // ensure area has background and foreground colors
        temperatureWidget->area->backgroundColor = PARAM_COLOR_WHITE;
        temperatureWidget->area->color = COLOR_TEMPERATURE;
    }

    if (humidityWidget) {
        humidityWidget->area = new Area(SENSOR_WIDGET_MARGIN,
                                        PARAM_SCREEN_HEIGHT - SENSOR_WIDGET_HEIGHT - SENSOR_WIDGET_MARGIN,
                                        SENSOR_WIDGET_WIDTH,
                                        SENSOR_WIDGET_HEIGHT);
        humidityWidget->area->backgroundColor = PARAM_COLOR_WHITE;
        humidityWidget->area->color = COLOR_HUMIDITY;
    }

    if (pressureWidget) {
        pressureWidget->area = new Area(3 * SENSOR_WIDGET_MARGIN + SENSOR_WIDGET_WIDTH,
                                         HEADER_HEIGHT + SENSOR_WIDGET_MARGIN,
                                         SENSOR_WIDGET_WIDTH,
                                         SENSOR_WIDGET_HEIGHT);
        pressureWidget->area->backgroundColor = PARAM_COLOR_WHITE;
        pressureWidget->area->color = COLOR_PRESSURE;
    }

    if (co2Widget) {
        co2Widget->area = new Area(3 * SENSOR_WIDGET_MARGIN + SENSOR_WIDGET_WIDTH,
                                   PARAM_SCREEN_HEIGHT - SENSOR_WIDGET_HEIGHT - SENSOR_WIDGET_MARGIN,
                                   SENSOR_WIDGET_WIDTH,
                                   SENSOR_WIDGET_HEIGHT);
        co2Widget->area->backgroundColor = PARAM_COLOR_WHITE;
        co2Widget->area->color = COLOR_CO2;
    }

    if (timeWidget) {
        timeWidget->area = new Area(PARAM_SCREEN_WIDTH / 2 + HEADER_MARGIN, HEADER_MARGIN, PARAM_SCREEN_WIDTH / 2 - 2 * HEADER_MARGIN, HEADER_HEIGHT - 2 * HEADER_MARGIN);
        timeWidget->area->backgroundColor = PARAM_COLOR_BLUE;
        timeWidget->area->color = PARAM_COLOR_WHITE;
    }
    if (settingWidget) {
        settingWidget->area = new Area(HEADER_MARGIN, HEADER_MARGIN, PARAM_SCREEN_WIDTH / 2 - 2 * HEADER_MARGIN, HEADER_HEIGHT - 2 * HEADER_MARGIN);
        settingWidget->area->backgroundColor = PARAM_COLOR_BLUE;
        settingWidget->area->color = PARAM_COLOR_WHITE;
    }

    // set the initial current widget (temperature)
    currentWidget = static_cast<NavigableWidget*>(temperatureWidget);

    // initial paint for all widget areas via their update() method
    if (temperatureWidget) temperatureWidget->update();
    if (humidityWidget)    humidityWidget->update();
    if (pressureWidget)   pressureWidget->update();
    if (co2Widget)         co2Widget->update();
    if (timeWidget)        timeWidget->update();
    if (settingWidget)     settingWidget->update();

    GFX_drawLine(SENSOR_WIDGET_MARGIN, HEADER_HEIGHT, PARAM_SCREEN_WIDTH - SENSOR_WIDGET_MARGIN, HEADER_HEIGHT, PARAM_COLOR_WHITE);

    GFX_drawLine(SENSOR_WIDGET_MARGIN, PARAM_SCREEN_HEIGHT - SENSOR_WIDGET_HEIGHT - 2 * SENSOR_WIDGET_MARGIN, PARAM_SCREEN_WIDTH - SENSOR_WIDGET_MARGIN, PARAM_SCREEN_HEIGHT - SENSOR_WIDGET_HEIGHT - 2 * SENSOR_WIDGET_MARGIN, PARAM_COLOR_WHITE);

    GFX_drawLine(PARAM_SCREEN_WIDTH / 2, HEADER_HEIGHT + SENSOR_WIDGET_MARGIN, PARAM_SCREEN_WIDTH / 2, PARAM_SCREEN_HEIGHT - SENSOR_WIDGET_MARGIN, PARAM_COLOR_WHITE);

    GFX_flush();
}
MainWindow::~MainWindow()
{

}

void MainWindow::joystickMoved(uint8_t direction)
{
NavigableWidget* newWidget = nullptr;
 switch (direction)
 {
    case 0:
        newWidget = currentWidget->left;
        break;
    case 1:
        newWidget = currentWidget->down;
        break;
    case 2:
        newWidget = currentWidget->right;
        break;
    case 3:
        newWidget = currentWidget->up;
        break;
    default:
        break;
 }
}
