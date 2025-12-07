#ifndef TIME_H
#define TIME_H

#include <stdint.h>
#include "ds3231.h"

#ifdef __cplusplus
extern "C" {
#endif

#define SDA_PIN_CLOCK 18
#define SDL_PIN_CLOCK 19

static ds3231_t ds3231;

// Simple time representation
typedef struct {
    uint8_t month;   // 1-12
    uint8_t day;     // 1-31
    uint8_t hour;    // 0-23
    uint8_t minute;  // 0-59
    uint8_t second;  // 0-59
} Time;

Time getClockTime();
Time getClockTimeImpl();

void initClock();

#ifdef __cplusplus
}
#endif

#endif // TIME_H
