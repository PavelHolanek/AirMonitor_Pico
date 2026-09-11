#ifndef GRAPH_WIDGET_H
#define GRAPH_WIDGET_H

#include "core/Widget.h"
#include "GraphData.h"
#include "Base.h"

// One joystick flick moves the frame by a quarter of its width, the same
// distance as the gap between two labels on the time axis. The joystick fires
// once per deflection and does not repeat while held, so a step of a single
// interval would take twenty flicks to page the graph.
constexpr int32_t GRAPH_SCROLL_INTERVALS = 5;

class GraphWidget : public Widget
{
public:
    GraphWidget();
    virtual ~GraphWidget();

    void setQuantity(QUANTITY q);

    // Pushed in from gui_timeChanged(). Nothing is plotted before the first call.
    void setCurrentTime(Time time);

    // Back to following the current time. The scope is a display preference and
    // survives, the scrolled position does not.
    void resetView();

    // Negative goes into the past. Clamped to the frame that still holds data
    // on one side and to the current time on the other.
    void scrollBy(int32_t intervals);

    // Negative zooms in (shorter span), positive zooms out.
    void changeScope(int8_t delta);

    // False while scrolled into the past, where new samples must not repaint.
    bool followsCurrentTime() const { return useRecentData; }

    void update() override;

private:
    Time effectiveTimeTo() const;
    bool oldestSampleTime(Time* out) const;

    // Clamps the wanted right edge into the allowed range and switches back to
    // the recent mode when it lands on the current time. True when the view
    // actually moved - a repaint costs a full area redraw, so it is skipped
    // when a flick changes nothing.
    bool applyTimeTo(Time desired);

    bool buildInput(graph_input_t* input);
    bool computeValueRange(int32_t* outBottom, int32_t* outTop) const;

    QUANTITY quantity;

    uint8_t scope = 0U;
    bool useRecentData = true;  
    Time timeTo{1U, 1U, 0U, 0U, 0U};

    Time currentTime{1U, 1U, 0U, 0U, 0U};
    bool hasCurrentTime = false;

    graph_points_t points;
};

#endif // GRAPH_WIDGET_H
