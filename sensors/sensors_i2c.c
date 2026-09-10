#include "sensors_i2c.h"
#include "Pinout.h"
#include "pico/stdlib.h"
#include <stdbool.h>

#define SENSORS_I2C_INSTANCE i2c0

// Standard mode, 100 kHz
#define SENSORS_I2C_BAUDRATE (100 * 1000)

static bool busReady = false;

i2c_inst_t* sensors_i2c_init(void)
{
    // Bus is shared by 3 sensors, it wll be initialize be the first one
    if (!busReady)
    {
        i2c_init(SENSORS_I2C_INSTANCE, SENSORS_I2C_BAUDRATE);

        gpio_set_function(SENSORS_I2C_SDA_PIN, GPIO_FUNC_I2C);
        gpio_set_function(SENSORS_I2C_SCL_PIN, GPIO_FUNC_I2C);
        gpio_pull_up(SENSORS_I2C_SDA_PIN);
        gpio_pull_up(SENSORS_I2C_SCL_PIN);

        busReady = true;
    }

    return SENSORS_I2C_INSTANCE;
}
