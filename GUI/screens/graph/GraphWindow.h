#ifndef GRAPH_WINDOW_H
#define GRAPH_WINDOW_H

#include "core/Window.h"
#include "core/GraphicElement.h"
#include "core/HoldRepeat.h"
#include "GraphWidget.h"
#include "GraphData.h"

// Holding left or right keeps scrolling: the first repeat after the delay, then
// one every period.
#define GRAPH_REPEAT_DELAY_MS 700U
#define GRAPH_REPEAT_PERIOD_MS 500U

class GraphWindow : public Window
{
public:
    GraphWindow();
    virtual ~GraphWindow();

    void setQuantity(QUANTITY q);
    QUANTITY getQuantity() const;
    void setCurrentTime(Time time);
    void updateData();

    void joystickEvent(const JoystickEvent& event) override;
    void timerExpired(GuiTimer* timer) override;
    void enterWindow() override;

private:
    void updateTitle();
    void scroll(JOYSTICK_DIRECTION direction);

    QUANTITY quantity;
    GraphWidget* graphWidget;
    Text* titleText;
    wchar_t titleBuffer[32];

    HoldRepeat scrollRepeat;
};

#endif // GRAPH_WINDOW_H
