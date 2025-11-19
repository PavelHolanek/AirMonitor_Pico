#include "Clock.h"
#include "Time.h"

void getTimeTask(void) {
    // For now, just call getTime(). Synchronization with RTC will be added later.
    Time t = getClockTime();
    (void)t; // suppress unused variable warning for now
}