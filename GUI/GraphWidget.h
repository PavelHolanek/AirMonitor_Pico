#ifndef GRAPH_WIDGET_H
#define GRAPH_WIDGET_H

#include "Widget.h"
#include "GraphData.h"
#include "Base.h"

typedef struct {
    uint16_t x;
    uint16_t y;
    bool valid;
} GraphCoordinate;

class GraphWidget : public Widget
{
public:
    GraphWidget();
    virtual ~GraphWidget();

    void setQuantity(QUANTITY q);
    void setData(const gui_graph_sample_t* data, size_t count);
    void setTimeFrame(Time fromTime, Time toTime);
    void update() override;

private:
    void computeCoordinates();
    void getScaleRange(int32_t* outBottom, int32_t* outTop) const;

    QUANTITY quantity;
    const gui_graph_sample_t* data;
    size_t dataCount;
    Time fromTime;
    Time toTime;
    GraphCoordinate coordinates[GRAPH_WIDGET_MAX_POINTS];
};

#endif // GRAPH_WIDGET_H
