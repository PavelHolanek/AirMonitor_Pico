//For C interface
//#include "GUIManager.c"

//must have the same name and signature as GUIManager.h

#include "../Base.h"

extern void gui_init();

extern void gui_dataChanged(QUANTITY type, int32_t value);

extern void gui_timeChanged(Time CurerntTime);

extern void gui_joystick(struct JoystickState state);

//extern void gui_showErrorMessage(const char* message);
