#ifndef TIME_H
#define TIME_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "ds3231.h"

#ifdef __cplusplus
extern "C" {
#endif

#define SDA_PIN_CLOCK 18
#define SDL_PIN_CLOCK 19

static ds3231_t ds3231;

// Simple time representation
typedef struct
{
    uint8_t month;   // 1-12
    uint8_t day;     // 1-31
    uint8_t hour;    // 0-23
    uint8_t minute;  // 0-59
    uint8_t second;  // 0-59
} Time;

Time getClockTime();
Time getClockTimeImpl();

void initClock();
void setClockTime(Time time);
void setClockTimeImpl(Time time);
uint32_t timeToTotalSeconds(Time time);
int8_t compareTimes(Time lhs, Time rhs);
bool isValidTime(Time time);
int32_t diffSeconds(Time lhs, Time rhs);
Time addTime(Time time, int32_t deltaDays, int32_t deltaHours, int32_t deltaMinutes, int32_t deltaSeconds);
Time addSeconds(Time time, int32_t deltaSeconds);
bool isTimeInRange(Time value, Time start, Time end);
const char* formatTime(Time time, char* buffer, size_t length);

#ifdef __cplusplus
}
#endif

#endif // TIME_H
