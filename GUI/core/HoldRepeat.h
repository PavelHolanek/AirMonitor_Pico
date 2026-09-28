#ifndef HOLD_REPEAT_H
#define HOLD_REPEAT_H

#include <stdint.h>
#include "Joystick.h"
#include "GuiTimer.h"

// Acts once on a flick, then again and again while the stick stays there. The
// owner does the first step itself, then calls start(); any other direction,
// the centre included, calls stop(). The timings belong to the owner.
class HoldRepeat
{
public:
    HoldRepeat(GuiTimerListener* owner, uint32_t firstDelayMs, uint32_t periodMs);

    void start(JOYSTICK_DIRECTION direction);
    void stop();

    // For timerExpired(): the held direction when the timer is ours and the
    // stick still points that way, JOYSTICK_NONE otherwise.
    JOYSTICK_DIRECTION expired(GuiTimer* expiredTimer);

private:
    GuiTimer timer;
    JOYSTICK_DIRECTION held;
    uint32_t firstDelayMs;
    uint32_t periodMs;
};

#endif // HOLD_REPEAT_H
