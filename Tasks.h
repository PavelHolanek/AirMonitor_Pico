#ifndef TASKS_H
#define TASKS_H

#ifdef __cplusplus
extern "C" {
#endif

#include "Clock.h"

// Placeholder for a FreeRTOS-related task function.
// For now it simply calls getTime(). Synchronization and RTC usage will be added later.
void getClockTimeTask(void);

#ifdef __cplusplus
}
#endif

#endif // TASKS_H
