#ifndef GRAPH_WIDGET_H
#define GRAPH_WIDGET_H

#include "Widget.h"
#include "GraphData.h"
#include "Base.h"

class GraphWidget : public Widget
{
public:
    GraphWidget();
    virtual ~GraphWidget();

    void setQuantity(QUANTITY q);

    // Pushed in from gui_timeChanged(). Nothing is plotted before the first call.
    void setCurrentTime(Time time);

    void update() override;

private:
    bool buildInput(graph_input_t* input);
    void getScaleRange(int32_t* outBottom, int32_t* outTop) const;

    QUANTITY quantity;

    // TODO: scope selection and the scrolled-into-the-past mode (useRecentData
    // == false, explicit timeTo) are wired up but not reachable from the GUI yet.
    uint8_t scope = 0U;
    bool useRecentData = true;
    Time timeTo{1U, 1U, 0U, 0U, 0U};

    Time currentTime{1U, 1U, 0U, 0U, 0U};
    bool hasCurrentTime = false;

    graph_points_t points;
};

#endif // GRAPH_WIDGET_H
