#include "Log.h"
#include <stdio.h>
#include "Clock.h"

void LOG(const char* message) {
    const Time* time = NULL;  //RTC not implemented yet
    if (!time) {
        printf("%s\n", message ? message : "");
        return;
    }

    printf("[%02u-%02u %02u:%02u:%02u] %s\n",
           (unsigned)time->month,
           (unsigned)time->day,
           (unsigned)time->hour,
           (unsigned)time->minute,
           (unsigned)time->second,
           message ? message : "");
}
