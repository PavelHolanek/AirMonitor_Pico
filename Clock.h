#ifndef TIME_H
#define TIME_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

// Simple time representation
typedef struct {
    uint8_t month;   // 1-12
    uint8_t day;     // 1-31
    uint8_t hour;    // 0-23
    uint8_t minute;  // 0-59
    uint8_t second;  // 0-59
} Time;

// Returns the current time.
// Currently a stub that will be implemented later.
Time getClockTime(void);

#ifdef __cplusplus
}
#endif

#endif // TIME_H
