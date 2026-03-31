#include "Clock.h"
#include "Log.h"
#include "Tasks.h"
#include "FreeRTOS.h"
#include "queue.h"
#include "semphr.h"
#include <stdio.h>

static const uint16_t daysBeforeMonth[12] =
{
    0,   // Jan
    31,  // Feb
    59,  // Mar
    90,  // Apr
    120, // May
    151, // Jun
    181, // Jul
    212, // Aug
    243, // Sep
    273, // Oct
    304, // Nov
    334  // Dec
};

static const uint8_t daysInMonth[12] =
{
    31, // Jan
    28, // Feb
    31, // Mar
    30, // Apr
    31, // May
    30, // Jun
    31, // Jul
    31, // Aug
    30, // Sep
    31, // Oct
    30, // Nov
    31  // Dec
};

static const uint32_t secondsInYear = 365U * 24U * 60U * 60U;

static Time totalSecondsToTime(uint32_t totalSeconds)
{
    Time out =
    {
        .month = 1U,
        .day = 1U,
        .hour = 0U,
        .minute = 0U,
        .second = 0U
    };

    uint32_t remainingSeconds = totalSeconds % secondsInYear;
    uint32_t remainingDays = remainingSeconds / (24U * 60U * 60U);
    remainingSeconds %= (24U * 60U * 60U);

    uint8_t monthIndex = 0U;
    while (monthIndex < 12U && remainingDays >= daysInMonth[monthIndex])
    {
        remainingDays -= daysInMonth[monthIndex];
        monthIndex++;
    }

    if (monthIndex < 12U)
    {
        out.month = (uint8_t)(monthIndex + 1U);
        out.day = (uint8_t)(remainingDays + 1U);
    }

    out.hour = (uint8_t)(remainingSeconds / 3600U);
    remainingSeconds %= 3600U;
    out.minute = (uint8_t)(remainingSeconds / 60U);
    out.second = (uint8_t)(remainingSeconds % 60U);
    return out;
}

Time getClockTime()
{
    Time value;
    xSemaphoreGive(TimeRequestSemaphore);
    xQueueReceive(CurrentTimeQueue, ( void * ) &value, TICKS_TO_WAIT);
    return value;
}

Time getClockTimeImpl()
{
    ds3231_data_t ds3231_data;
    Time t;
    if (ds3231_read_current_time(&ds3231, &ds3231_data)) 
    {
        t.month = 0;
        t.day =  0;
        t.hour = 0;
        t.minute = 0;
        t.second = 0;
    }
    else
    {
        t.month = ds3231_data.month;
        t.day = ds3231_data.date;
        t.hour = ds3231_data.hours;
        t.minute = ds3231_data.minutes;
        t.second = ds3231_data.seconds;
    }
    return t;
}

void initClock()
{
    ds3231_init(&ds3231, &i2c1_inst, DS3231_DEVICE_ADRESS, AT24C32_EEPROM_ADRESS_0);
    sleep_ms(200);
    gpio_init(SDA_PIN_CLOCK);
    gpio_init(SDL_PIN_CLOCK);
    gpio_set_function(SDA_PIN_CLOCK, GPIO_FUNC_I2C);
    gpio_set_function(SDL_PIN_CLOCK, GPIO_FUNC_I2C);
    gpio_pull_up(SDA_PIN_CLOCK);
    gpio_pull_up(SDL_PIN_CLOCK);
    i2c_init(ds3231.i2c, 400 * 1000);
}

void setClockTimeImpl(Time time)
{
    ds3231_data_t ds3231_data =
    {
        .seconds = time.second,
        .minutes = time.minute,
        .hours = time.hour,
        .day = 0,             // day of week not provided; keep 0
        .date = time.day,      // day of month
        .month = time.month,
        .year = 25,            // TODO: Year not in Time; keep current default
        .century = 1,
        .am_pm = false
    };
    ds3231_configure_time(&ds3231, &ds3231_data);
}

void setClockTime(Time time)
{
    // Enqueue desired time and signal the setting task
    xQueueOverwrite(TimeToSetQueue, (void*)&time);
    xSemaphoreGive(TimeSetRequestSemaphore);
}

bool isValidTime(Time time)
{
    if (time.month < 1U || time.month > 12U)
    {
        return false;
    }
    if (time.hour > 23U || time.minute > 59U || time.second > 59U)
    {
        return false;
    }

    const uint8_t maxDay = daysInMonth[time.month - 1U];
    return (time.day >= 1U && time.day <= maxDay);
}

uint32_t timeToTotalSeconds(Time t)
{
    uint8_t month = t.month;
    uint8_t day = t.day;
    uint8_t hour = t.hour;
    uint8_t minute = t.minute;
    uint8_t second = t.second;

    if (month < 1U || month > 12U) month = 1U;
    if (day < 1U || day > 31U) day = 1U;
    if (hour > 23U) hour = 23U;
    if (minute > 59U) minute = 59U;
    if (second > 59U) second = 59U;

    const uint32_t days = (uint32_t)daysBeforeMonth[month - 1U] + (uint32_t)(day - 1U);
    return (((days * 24U) + (uint32_t)hour) * 60U + (uint32_t)minute) * 60U + (uint32_t)second;
}

int8_t compareTimes(Time lhs, Time rhs)
{
    if (lhs.month < rhs.month) return -1;
    if (lhs.month > rhs.month) return 1;

    if (lhs.day < rhs.day) return -1;
    if (lhs.day > rhs.day) return 1;

    if (lhs.hour < rhs.hour) return -1;
    if (lhs.hour > rhs.hour) return 1;

    if (lhs.minute < rhs.minute) return -1;
    if (lhs.minute > rhs.minute) return 1;

    if (lhs.second < rhs.second) return -1;
    if (lhs.second > rhs.second) return 1;

    return 0;
}

int32_t diffSeconds(Time lhs, Time rhs)
{
    const uint32_t lhsSeconds = timeToTotalSeconds(lhs);
    const uint32_t rhsSeconds = timeToTotalSeconds(rhs);
    return (int32_t)lhsSeconds - (int32_t)rhsSeconds;
}

Time addTime(Time time, int32_t deltaDays, int32_t deltaHours, int32_t deltaMinutes, int32_t deltaSeconds)
{
    const int64_t base = (int64_t)timeToTotalSeconds(time);
    int64_t total = base;
    total += (int64_t)deltaDays * 24LL * 60LL * 60LL;
    total += (int64_t)deltaHours * 60LL * 60LL;
    total += (int64_t)deltaMinutes * 60LL;
    total += (int64_t)deltaSeconds;

    total %= (int64_t)secondsInYear;
    if (total < 0)
    {
        total += (int64_t)secondsInYear;
    }
    return totalSecondsToTime((uint32_t)total);
}

Time addSeconds(Time time, int32_t deltaSeconds)
{
    return addTime(time, 0, 0, 0, deltaSeconds);
}

bool isTimeInRange(Time value, Time start, Time end)
{
    const int8_t startToEnd = compareTimes(start, end);
    if (startToEnd <= 0)
    {
        return compareTimes(value, start) >= 0 && compareTimes(value, end) <= 0;
    }
    return compareTimes(value, start) >= 0 || compareTimes(value, end) <= 0;
}

const char* formatTime(Time time, char* buffer, size_t length)
{
    if (buffer == NULL || length == 0U)
    {
        return NULL;
    }

    (void)snprintf(
        buffer,
        length,
        "%02u/%02u %02u:%02u:%02u",
        (unsigned int)time.month,
        (unsigned int)time.day,
        (unsigned int)time.hour,
        (unsigned int)time.minute,
        (unsigned int)time.second
    );
    return buffer;
}
