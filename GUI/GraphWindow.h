#ifndef GRAPH_WINDOW_H
#define GRAPH_WINDOW_H

#include "Window.h"
#include "GraphWidget.h"
#include "GraphData.h"

class GraphWindow : public Window
{
public:
    GraphWindow();
    virtual ~GraphWindow();

    void setQuantity(QUANTITY q);
    QUANTITY getQuantity() const;
    void setGraphData(QUANTITY type, const gui_graph_sample_t* data, size_t count, Time fromTime, Time toTime);
    void updateGraph();

    void joystickAction(JoystickState state) override;
    void enterWindow() override;

private:
    void updateTitle();

    QUANTITY quantity;
    GraphWidget* graphWidget;
    Text* titleText;
    wchar_t titleBuffer[32];
};

#endif // GRAPH_WINDOW_H
