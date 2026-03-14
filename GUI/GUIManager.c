//For C interface
//#include "GUIManager.c"

//must have the same name and signature as GUIManager.h

#include "../Base.h"
#include "GraphData.h"
#include "Clock.h"

extern void gui_init();

extern void gui_dataChanged(QUANTITY type, int32_t value);
extern void gui_graphDataChanged(QUANTITY type,
                                 const gui_graph_sample_t* data,
                                 size_t count,
                                 Time fromTime,
                                 Time toTime);

extern void gui_timeChanged(Time CurerntTime);

extern void gui_joystick(struct JoystickState state);

extern void gui_idleTimePassed();

//extern void gui_showErrorMessage(const char* message);
