//For C interface
//#include "GUIManager_c.h"

//must have the same name and signature as GUIManager.h

#include <stdint.h>
#include "Base.h"
#include "Clock.h"
#include "GuiEvent.h"

extern void gui_init();

extern void gui_dataChanged();

extern void gui_timeChanged(Time CurerntTime);

extern void gui_handleEvent(const GuiEvent* event);

// UINT32_MAX when no GUI timer is armed
extern uint32_t gui_msUntilNextTimer();

extern void gui_processTimers();
