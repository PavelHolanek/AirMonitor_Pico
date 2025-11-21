#include "Clock.h"
#include "Log.h"

Time getClockTime() {
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
        t.day = ds3231_data.day;
        t.hour = ds3231_data.hours;
        t.minute = ds3231_data.minutes;
        t.second = ds3231_data.seconds;
    }
    return t;
}

void initClock()
{
    ds3231_init(&ds3231, i2c_default, DS3231_DEVICE_ADRESS, AT24C32_EEPROM_ADRESS_0);
    sleep_ms(200);

    /* Initiliaze I2C line. */
    gpio_init(SDA_PIN_CLOCK);
    gpio_init(SDL_PIN_CLOCK);
    gpio_set_function(SDA_PIN_CLOCK, GPIO_FUNC_I2C);
    gpio_set_function(SDL_PIN_CLOCK, GPIO_FUNC_I2C);
    gpio_pull_up(SDA_PIN_CLOCK);
    gpio_pull_up(SDL_PIN_CLOCK);
    i2c_init(ds3231.i2c, 400 * 1000);

    ds3231_data_t ds3231_data = {
        .seconds = 25,
        .minutes = 30,
        .hours = 1,
        .day = 5,
        .date = 21,
        .month = 11,
        .year = 25,
        .century = 1,
        .am_pm = false
    };

    ds3231_configure_time(&ds3231, &ds3231_data);

    LOG("Clock initialized");
}