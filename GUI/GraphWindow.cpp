#include "GraphWindow.h"
#include "GUIManager.h"
#include "Parameters.h"
#include "Libraries/pico-displayDrivs/gfx/gfx.h"
#include <wchar.h>

GraphWindow::GraphWindow()
    : Window(),
      quantity(QUANTITY_TEMPERATURE),
      graphWidget(new GraphWidget()),
      titleText(new Text(L"")),
      titleBuffer{0}
{
    if (graphWidget)
    {
        graphWidget->area = new Area(20, 60, PARAM_SCREEN_WIDTH - 40, PARAM_SCREEN_HEIGHT - 80);
        graphWidget->area->backgroundColor = PARAM_COLOR_BLACK;
        graphWidget->area->color = PARAM_COLOR_WHITE;
    }

    if (titleText)
    {
        titleText->textSize = 3;
        titleText->backgroundColor = PARAM_COLOR_BLACK;
        titleText->color = PARAM_COLOR_WHITE;
    }
}

GraphWindow::~GraphWindow()
{
    if (graphWidget)
    {
        delete graphWidget;
        graphWidget = nullptr;
    }
    if (titleText)
    {
        delete titleText;
        titleText = nullptr;
    }
}

void GraphWindow::setQuantity(QUANTITY q)
{
    quantity = q;
    if (graphWidget)
    {
        graphWidget->setQuantity(q);
    }
}

QUANTITY GraphWindow::getQuantity() const
{
    return quantity;
}

void GraphWindow::setCurrentTime(Time time)
{
    if (graphWidget)
    {
        graphWidget->setCurrentTime(time);
    }
}

void GraphWindow::updateData()
{
    graphWidget->update(); 
}


void GraphWindow::updateTitle()
{
    const wchar_t* quantityName = L"";

    const wchar_t* units = L"";

    switch (quantity)
    {
        case QUANTITY_TEMPERATURE: quantityName = L"Temperature"; units = L"C"; break;
        case QUANTITY_HUMIDITY: quantityName = L"Humidity"; units = L"%%"; break;
        case QUANTITY_PRESSURE: quantityName = L"Pressure"; units = L"hPa"; break;
        case QUANTITY_CO2: quantityName = L"CO2"; units = L"ppm"; break;
        default: quantityName = L"Quantity"; units = L"-"; break;
    }

    swprintf(titleBuffer, sizeof(titleBuffer) / sizeof(titleBuffer[0]), L"%ls [%ls]", quantityName, units);
    titleText->str = titleBuffer;
    titleText->posX = 20;
    titleText->posY = 16;
}

void GraphWindow::enterWindow()
{
    GFX_fillScreen(PARAM_COLOR_BLACK);

    updateTitle();
    if (titleText)
    {
        titleText->Paint();
    }

    if (graphWidget)
    {
        graphWidget->update();
    }
}

void GraphWindow::joystickAction(JoystickState state)
{
    if (state.pressed)
    {
        gui_changeWindow(mainWindow);
    }
}
