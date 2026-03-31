//For C interface
//#include "GUIManager_c.h"

//must have the same name and signature as GUIManager.h

#include "../Base.h"
#include "GraphData.h"
#include "Clock.h"

extern void gui_init();

extern void gui_dataChanged();

extern void gui_timeChanged(Time CurerntTime);

extern void gui_joystick(struct JoystickState state);

extern void gui_idleTimePassed();
