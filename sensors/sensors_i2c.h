#ifndef SENSORS_I2C_H
#define SENSORS_I2C_H

#include "hardware/i2c.h"

#ifdef __cplusplus
extern "C" {
#endif

// Brings up the I2C bus the BMP280, the SHT40 and the SCD41 all share, and returns it
i2c_inst_t* sensors_i2c_init(void);

#ifdef __cplusplus
}
#endif

#endif // SENSORS_I2C_H
