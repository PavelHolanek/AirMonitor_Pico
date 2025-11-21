#include "Log.h"
#include <stdio.h>
#include "Clock.h"

void LOG(const char* message) {
    const Time time = getClockTime();  //RTC not implemented yet
    printf("[%02u-%02u %02u:%02u:%02u] %s\n",
           (unsigned)time.month,
           (unsigned)time.day,
           (unsigned)time.hour,
           (unsigned)time.minute,
           (unsigned)time.second,
           message ? message : "");
}
