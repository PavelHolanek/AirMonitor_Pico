#ifndef GUI_EVENT_H
#define GUI_EVENT_H

// One entry of the queue guiTask serves. Shared by the C and the C++ side.

#include "Joystick.h"

typedef enum
{
    GUI_EVENT_JOYSTICK = 0
} GUI_EVENT_TYPE;

// A new source gets its own type and its own member of the union.
typedef struct
{
    GUI_EVENT_TYPE type;
    union
    {
        JoystickEvent joystick;
    };
} GuiEvent;

#endif // GUI_EVENT_H
