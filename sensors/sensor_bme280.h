/*
 * Sensor driver skeleton for BME280
 * This is a stub interface. Replace with actual implementation as needed.
 */
#ifndef SENSOR_BME280_H
#define SENSOR_BME280_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>

/* Basic measurement set for BME280 (T + RH + Pressure) */
typedef struct {
    int32_t temperature_c;
    int32_t humidity_rh;
    int32_t pressure_pa;
} sensor_bme280_data_t;

/* Initialize the sensor (I2C, pins, etc.). Return true on success. */
bool sensor_bme280_init(void);

/* Read a single measurement. Returns true on success, false otherwise. */
bool sensor_bme280_read(sensor_bme280_data_t* out);

/* Optional: put sensor into low-power or free resources. */
void sensor_bme280_deinit(void);

#ifdef __cplusplus
}
#endif

#endif /* SENSOR_BME280_H */
