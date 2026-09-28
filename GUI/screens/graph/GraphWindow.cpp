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
      titleBuffer{0},
      scrollRepeat(this, GRAPH_REPEAT_DELAY_MS, GRAPH_REPEAT_PERIOD_MS)
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
    if (graphWidget && graphWidget->followsCurrentTime())
    {
        graphWidget->update();
    }
}


void GraphWindow::updateTitle()
{
    const wchar_t* quantityName = L"";

    const wchar_t* units = L"";

    switch (quantity)
    {
        case QUANTITY_TEMPERATURE: quantityName = L"Teplota"; units = L"C"; break;
        case QUANTITY_HUMIDITY: quantityName = L"Vlhkost"; units = L"%%"; break;
        case QUANTITY_PRESSURE: quantityName = L"Tlak"; units = L"hPa"; break;
        case QUANTITY_CO2: quantityName = L"CO2"; units = L"ppm"; break;
        default: quantityName = L"Veličina"; units = L"-"; break;
    }

    swprintf(titleBuffer, sizeof(titleBuffer) / sizeof(titleBuffer[0]), L"%ls [%ls]", quantityName, units);
    titleText->str = titleBuffer;
    titleText->posX = 20;
    titleText->posY = 16;
}

void GraphWindow::enterWindow()
{
    Area* plot = (graphWidget != nullptr) ? graphWidget->area : nullptr;

    if (plot == nullptr)
    {
        GFX_fillScreen(PARAM_COLOR_BLACK);
    }
    else
    {
        // graphWidget->update() repaints its whole area, so only what falls
        // outside it needs wiping here - clearing the full screen first would
        // push most of the panel over SPI twice.
        GFX_fillRect(0, plot->getEndY(), PARAM_SCREEN_WIDTH,
                     PARAM_SCREEN_HEIGHT - plot->getEndY(), PARAM_COLOR_BLACK);
        GFX_fillRect(0, plot->posY, plot->posX, plot->sizeY, PARAM_COLOR_BLACK);
        GFX_fillRect(plot->getEndX(), plot->posY,
                     PARAM_SCREEN_WIDTH - plot->getEndX(), plot->sizeY, PARAM_COLOR_BLACK);
    }

    // The strip above the graph holds the title, so it is worth a buffer of its
    // own: at text size 3 every glyph is forty little blits without one.
    const uint16_t headerHeight = (plot != nullptr) ? plot->posY : 0U;
    const bool ownsFramebuf = (headerHeight > 0U)
                              && GFX_createFramebuf(0, 0, PARAM_SCREEN_WIDTH, headerHeight);
    if (ownsFramebuf)
    {
        GFX_clearFramebuf(PARAM_COLOR_BLACK);
    }
    else if (headerHeight > 0U)
    {
        GFX_fillRect(0, 0, PARAM_SCREEN_WIDTH, headerHeight, PARAM_COLOR_BLACK);
    }

    updateTitle();
    if (titleText)
    {
        titleText->Paint();
    }

    if (ownsFramebuf)
    {
        GFX_flush();
        GFX_destroyFramebuf();
    }

    if (graphWidget)
    {
        graphWidget->resetView();
        graphWidget->update();
    }
}

void GraphWindow::joystickEvent(const JoystickEvent& event)
{
    if (event.type == JOYSTICK_EVENT_BUTTON_DOWN)
    {
        gui_changeWindow(mainWindow);
        return;
    }

    if (!graphWidget || event.type != JOYSTICK_EVENT_DIRECTION)
    {
        return;
    }

    // Any change ends the repeat, the centre included.
    scrollRepeat.stop();

    switch (event.direction)
    {
        case JOYSTICK_RIGHT:
        case JOYSTICK_LEFT:
            scroll(event.direction);
            scrollRepeat.start(event.direction);
            break;
        case JOYSTICK_UP: // only six scopes - the scope does not repeat
            graphWidget->changeScope(1);
            break;
        case JOYSTICK_DOWN:
            graphWidget->changeScope(-1);
            break;
        default: // JOYSTICK_NONE - back in the centre
            break;
    }
}

void GraphWindow::scroll(JOYSTICK_DIRECTION direction)
{
    graphWidget->scrollBy((direction == JOYSTICK_RIGHT) ? GRAPH_SCROLL_INTERVALS : -GRAPH_SCROLL_INTERVALS);
}

void GraphWindow::timerExpired(GuiTimer* timer)
{
    const JOYSTICK_DIRECTION held = scrollRepeat.expired(timer);
    if (held != JOYSTICK_NONE && graphWidget)
    {
        scroll(held);
    }
}
