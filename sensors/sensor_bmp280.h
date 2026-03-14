/*
 * Sensor driver skeleton for bmp280
 * This is a stub interface. Replace with actual implementation as needed.
 */
#ifndef SENSOR_bmp280_H
#define SENSOR_bmp280_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>

/* Basic measurement set for bmp280 (T + RH + Pressure) */
typedef struct
{
    int32_t temperature_c;
    int32_t humidity_rh;
    int32_t pressure_pa;
} sensor_bmp280_data_t;

/* Initialize the sensor (I2C, pins, etc.). Return true on success. */
bool sensor_bmp280_init(void);

/* Read a single measurement. Returns true on success, false otherwise. */
bool sensor_bmp280_read(sensor_bmp280_data_t* out);

/* Optional: put sensor into low-power or free resources. */
void sensor_bmp280_deinit(void);

#ifdef __cplusplus
}
#endif

#endif /* SENSOR_bmp280_H */
