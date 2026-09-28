#include "GuiTimer.h"
#include "pico/time.h"

GuiTimer* GuiTimer::first = nullptr;

static uint32_t nowMs()
{
    return to_ms_since_boot(get_absolute_time());
}

// Signed difference, so the milliseconds wrapping after 49 days do not matter.
static bool isDue(uint32_t deadline, uint32_t now)
{
    return (int32_t)(now - deadline) >= 0;
}

GuiTimer::GuiTimer(GuiTimerListener* owner)
    : owner(owner), next(first)
{
    first = this;
}

GuiTimer::~GuiTimer()
{
    for (GuiTimer** link = &first; *link; link = &(*link)->next)
    {
        if (*link == this)
        {
            *link = next;
            break;
        }
    }
}

void GuiTimer::start(uint32_t delayMs, uint32_t period)
{
    deadlineMs = nowMs() + delayMs;
    periodMs = period;
    running = true;
}

void GuiTimer::stop()
{
    running = false;
}

uint32_t GuiTimer::msUntilNext()
{
    const uint32_t now = nowMs();
    uint32_t nearest = UINT32_MAX;

    for (GuiTimer* timer = first; timer; timer = timer->next)
    {
        if (!timer->running)
        {
            continue;
        }
        const uint32_t remaining = isDue(timer->deadlineMs, now) ? 0U : (uint32_t)(timer->deadlineMs - now);
        if (remaining < nearest)
        {
            nearest = remaining;
        }
    }
    return nearest;
}

void GuiTimer::processExpired()
{
    const uint32_t now = nowMs();

    for (GuiTimer* timer = first; timer; timer = timer->next)
    {
        if (!timer->running || !isDue(timer->deadlineMs, now))
        {
            continue;
        }

        if (timer->periodMs == 0U)
        {
            timer->running = false;
        }
        else
        {
            // From the previous deadline, so the period does not drift. A
            // deadline missed behind a long redraw is skipped, not caught up.
            timer->deadlineMs += timer->periodMs;
            if (isDue(timer->deadlineMs, now))
            {
                timer->deadlineMs = now + timer->periodMs;
            }
        }

        // Last, because the listener may re-arm this timer or switch windows.
        if (timer->owner)
        {
            timer->owner->timerExpired(timer);
        }
    }
}

void GuiTimer::stopAllOwnedBy(const GuiTimerListener* owner)
{
    for (GuiTimer* timer = first; timer; timer = timer->next)
    {
        if (timer->owner == owner)
        {
            timer->running = false;
        }
    }
}
