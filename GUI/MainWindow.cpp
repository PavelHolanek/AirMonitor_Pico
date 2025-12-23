// MainWindow.cpp - implementation of MainWindow
#include "MainWindow.h"
#include "Parameters.h"
#include "Libraries/pico-displayDrivs/gfx/gfx.h"
#include "stdio.h"
#include "GUIManager.h"
MainWindow::MainWindow()
{
    // create sensor widgets with their QUANTITY types
    temperatureWidget = new QuantityWidget(QUANTITY_TEMPERATURE);
    humidityWidget    = new QuantityWidget(QUANTITY_HUMIDITY);
    pressureWidget   = new QuantityWidget(QUANTITY_PRESSURE);
    co2Widget         = new QuantityWidget(QUANTITY_CO2);

    // time and settings are generic navigable widgets
    timeWidget    = new TimeWidget();
    settingWidget = new SettingsWidget();

    // create simple area layouts for each widget (positions/sizes are examples)
    if (temperatureWidget) {
        temperatureWidget->area = new Area(SENSOR_WIDGET_MARGIN,
                                           HEADER_HEIGHT + SENSOR_WIDGET_MARGIN,
                                           SENSOR_WIDGET_WIDTH,
                                           SENSOR_WIDGET_HEIGHT);
        // ensure area has background and foreground colors
        temperatureWidget->area->backgroundColor = PARAM_COLOR_BLACK;
        temperatureWidget->area->color = COLOR_TEMPERATURE;
    }

    if (humidityWidget) {
        humidityWidget->area = new Area(SENSOR_WIDGET_MARGIN,
                                        PARAM_SCREEN_HEIGHT - SENSOR_WIDGET_HEIGHT - SENSOR_WIDGET_MARGIN,
                                        SENSOR_WIDGET_WIDTH,
                                        SENSOR_WIDGET_HEIGHT);
        humidityWidget->area->backgroundColor = PARAM_COLOR_BLACK;
        humidityWidget->area->color = COLOR_HUMIDITY;
    }

    if (pressureWidget) {
        pressureWidget->area = new Area(3 * SENSOR_WIDGET_MARGIN + SENSOR_WIDGET_WIDTH,
                                         HEADER_HEIGHT + SENSOR_WIDGET_MARGIN,
                                         SENSOR_WIDGET_WIDTH,
                                         SENSOR_WIDGET_HEIGHT);
        pressureWidget->area->backgroundColor = PARAM_COLOR_BLACK;
        pressureWidget->area->color = COLOR_PRESSURE;
    }

    if (co2Widget) {
        co2Widget->area = new Area(3 * SENSOR_WIDGET_MARGIN + SENSOR_WIDGET_WIDTH,
                                   PARAM_SCREEN_HEIGHT - SENSOR_WIDGET_HEIGHT - SENSOR_WIDGET_MARGIN,
                                   SENSOR_WIDGET_WIDTH,
                                   SENSOR_WIDGET_HEIGHT);
        co2Widget->area->backgroundColor = PARAM_COLOR_BLACK;
        co2Widget->area->color = COLOR_CO2;
    }

    if (timeWidget) {
        timeWidget->area = new Area(PARAM_SCREEN_WIDTH / 2 + HEADER_MARGIN, HEADER_MARGIN, PARAM_SCREEN_WIDTH / 2 - 2 * HEADER_MARGIN, HEADER_HEIGHT - 2 * HEADER_MARGIN);
        timeWidget->area->backgroundColor = PARAM_COLOR_BLACK;
        timeWidget->area->color = PARAM_COLOR_WHITE;
    }
    if (settingWidget) {
        //settingWidget->area = new Area(HEADER_MARGIN, HEADER_MARGIN, PARAM_SCREEN_WIDTH / 2 - 2 * HEADER_MARGIN, HEADER_HEIGHT - 2 * HEADER_MARGIN);
        settingWidget->area = new Area(HEADER_MARGIN + SENSOR_WIDGET_PADDING, HEADER_MARGIN, 40, 40);
        settingWidget->area->backgroundColor = PARAM_COLOR_BLACK;
        settingWidget->area->color = PARAM_COLOR_WHITE;
    }

    // set the initial current widget (temperature)
    currentWidget = static_cast<NavigableWidget*>(temperatureWidget);

    // initial paint for all widget areas via their update() method
    
    if (temperatureWidget)  temperatureWidget->initialize();
    if (humidityWidget)     humidityWidget->initialize();
    if (pressureWidget)     pressureWidget->initialize();
    if (co2Widget)          co2Widget->initialize();

    // Wire up navigable neighbors for 2 columns x 3 rows grid
    // Row 1: settings | time
    if (settingWidget && timeWidget) {
        settingWidget->right = timeWidget;
        timeWidget->left = settingWidget;

        // Down from header row to sensor row 2
        settingWidget->down = temperatureWidget;
        timeWidget->down = pressureWidget;
    }

    // Row 2: temperature | pressure
    if (temperatureWidget && pressureWidget) {
        temperatureWidget->right = pressureWidget;
        pressureWidget->left = temperatureWidget;

        // Up links to header row
        temperatureWidget->up = settingWidget;
        pressureWidget->up = timeWidget;

        // Down links to row 3
        temperatureWidget->down = humidityWidget;
        pressureWidget->down = co2Widget;
    }

    // Row 3: humidity | co2
    if (humidityWidget && co2Widget) {
        humidityWidget->right = co2Widget;
        co2Widget->left = humidityWidget;

        // Up links to row 2
        humidityWidget->up = temperatureWidget;
        co2Widget->up = pressureWidget;
    }

    // Ensure invalid directions are nullptr (grid boundaries)
    // Row 1 boundaries
    if (settingWidget) { settingWidget->left = nullptr; settingWidget->up = nullptr; }
    if (timeWidget) { timeWidget->right = nullptr; timeWidget->up = nullptr; }

    // Row 2 boundaries
    if (temperatureWidget) { temperatureWidget->left = nullptr; }
    if (pressureWidget) { pressureWidget->right = nullptr; }

    // Row 3 boundaries
    if (humidityWidget) { humidityWidget->left = nullptr; humidityWidget->down = nullptr; }
    if (co2Widget) { co2Widget->right = nullptr; co2Widget->down = nullptr; }
}

void MainWindow::joystickAction(JoystickState state)
{
    if (state.pressed)
    {
        if (currentWidget == timeWidget)
        {
            gui_changeWindow(clockWindow);
        }
        return;
    }
    else if (currentWidget)
    {
        NavigableWidget* newWidget = nullptr;
        switch (getDominantState(state))
        {
            case 0: // right
                newWidget = currentWidget->right; break;
            case 1: // up
                newWidget = currentWidget->up; break;
            case 2: // left
                newWidget = currentWidget->left; break;
            default: // 3: down
                newWidget = currentWidget->down; break;
        }

        if (newWidget)
        {
            newWidget->selected();
            currentWidget->deselected();
            currentWidget = newWidget;
        }
        return;
    }
    else
    {
        currentWidget = timeWidget;
        currentWidget->selected();
        return;
    }
}

QuantityWidget* MainWindow::getWidgetByType(QUANTITY type)
{
    if(QUANTITY_TEMPERATURE == type)
    {
        return temperatureWidget;
    }
    else if(QUANTITY_CO2 == type)
    {
        return co2Widget;
    }
    else if(QUANTITY_HUMIDITY == type)
    {
        return humidityWidget;
    }
    else if(QUANTITY_PRESSURE == type)
    {
        return pressureWidget;
    }
    return nullptr;
}

void MainWindow::enterWindow()
{
    GFX_fillRect(0, 0, PARAM_SCREEN_WIDTH, PARAM_SCREEN_HEIGHT, MAIN_WINDOW_BACKGROUND);

    GFX_drawLine(SENSOR_WIDGET_MARGIN, HEADER_HEIGHT, PARAM_SCREEN_WIDTH - SENSOR_WIDGET_MARGIN, HEADER_HEIGHT, PARAM_COLOR_WHITE);

    GFX_drawLine(SENSOR_WIDGET_MARGIN, PARAM_SCREEN_HEIGHT - SENSOR_WIDGET_HEIGHT - 2 * SENSOR_WIDGET_MARGIN, PARAM_SCREEN_WIDTH - SENSOR_WIDGET_MARGIN, PARAM_SCREEN_HEIGHT - SENSOR_WIDGET_HEIGHT - 2 * SENSOR_WIDGET_MARGIN, PARAM_COLOR_WHITE);

    GFX_drawLine(PARAM_SCREEN_WIDTH / 2, HEADER_HEIGHT + SENSOR_WIDGET_MARGIN, PARAM_SCREEN_WIDTH / 2, PARAM_SCREEN_HEIGHT - SENSOR_WIDGET_MARGIN, PARAM_COLOR_WHITE);
    if (temperatureWidget) temperatureWidget->update();
    if (humidityWidget)    humidityWidget->update();
    if (pressureWidget)    pressureWidget->update();
    if (co2Widget)         co2Widget->update();
    if (timeWidget)        timeWidget->update();
    if (settingWidget)     settingWidget->update();
}

MainWindow::~MainWindow()
{
    
}
