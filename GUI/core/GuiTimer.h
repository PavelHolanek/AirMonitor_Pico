#ifndef GUI_TIMER_H
#define GUI_TIMER_H

#include <stdint.h>

class GuiTimer;

// Whoever a timer reports to - a window, or the manager for the idle timeout.
class GuiTimerListener
{
public:
    virtual ~GuiTimerListener() {}
    virtual void timerExpired(GuiTimer* timer) = 0;
};

// A deadline served by guiTask. The GUI never waits: it arms a timer, returns,
// and timerExpired() comes later like any other event.
//
// Start and stop only from code running on guiTask - joystick events and timer
// callbacks. updateData() and timeChanged run on other tasks, and a timer armed
// there would not wake guiTask until something else does.
class GuiTimer
{
public:
    explicit GuiTimer(GuiTimerListener* owner);
    ~GuiTimer();

    GuiTimer(const GuiTimer&) = delete;
    GuiTimer& operator=(const GuiTimer&) = delete;

    // periodMs 0 = one-shot. Starting a running timer re-arms it.
    void start(uint32_t delayMs, uint32_t periodMs = 0U);
    void stop();
    bool isRunning() const { return running; }

    // Driven by GUIManager.
    static uint32_t msUntilNext();  // UINT32_MAX when nothing is armed
    static void processExpired();
    static void stopAllOwnedBy(const GuiTimerListener* owner);

private:
    GuiTimerListener* owner;
    uint32_t deadlineMs = 0U;
    uint32_t periodMs = 0U;
    bool running = false;

    // Every timer ever made, so the engine needs no allocation. Windows live
    // for the whole run, and so do their timers.
    GuiTimer* next;
    static GuiTimer* first;
};

#endif // GUI_TIMER_H
