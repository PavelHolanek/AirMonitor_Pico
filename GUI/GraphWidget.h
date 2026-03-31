#ifndef GRAPH_WIDGET_H
#define GRAPH_WIDGET_H

#include "Widget.h"
#include "GraphData.h"
#include "Base.h"

typedef struct
{
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
    void computeTimeFrame();
    void update() override;

private:
    void computeCoordinates();
    void getScaleRange(int32_t* outBottom, int32_t* outTop) const;

    void setCurrentTime(Time time);
    void moveTimeFrameLeft(Time time);
    void moveTimeFrameRight(Time time);

    QUANTITY quantity;
    const gui_graph_sample_t* data;
    size_t dataCount;
    Time fromTime;
    Time toTime;
    GraphCoordinate coordinates[GRAPH_WIDGET_MAX_POINTS];

    uint8_t scope = 0; //0...SCOPES_COUNT

    Time currentTime;
    Time startTime;
    Time endTime;

    bool useRecentData = true;
};

#endif // GRAPH_WIDGET_H
