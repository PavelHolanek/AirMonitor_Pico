#include "HoldRepeat.h"

HoldRepeat::HoldRepeat(GuiTimerListener* owner, uint32_t firstDelayMs, uint32_t periodMs)
    : timer(owner), held(JOYSTICK_NONE), firstDelayMs(firstDelayMs), periodMs(periodMs)
{
}

void HoldRepeat::start(JOYSTICK_DIRECTION direction)
{
    if (direction == JOYSTICK_NONE)
    {
        stop();
        return;
    }
    held = direction;
    timer.start(firstDelayMs, periodMs);
}

void HoldRepeat::stop()
{
    held = JOYSTICK_NONE;
    timer.stop();
}

JOYSTICK_DIRECTION HoldRepeat::expired(GuiTimer* expiredTimer)
{
    if (expiredTimer != &timer || held == JOYSTICK_NONE)
    {
        return JOYSTICK_NONE;
    }

    // The stick may already be back while its event still waits in the queue.
    JoystickSnapshot state;
    joystick_getState(&state);
    if (state.direction != held)
    {
        stop();
        return JOYSTICK_NONE;
    }
    return held;
}
