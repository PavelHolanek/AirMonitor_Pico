/*
 * Sensor driver skeleton for SHT40
 * This is a stub interface. Replace with actual implementation as needed.
 */
#ifndef SENSOR_SHT40_H
#define SENSOR_SHT40_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>

/* Basic measurement set for SHT40 (T + RH) */
typedef struct {
    int32_t temperature_c;
    int32_t humidity_rh;
} sensor_sht40_data_t;

/* Initialize the sensor (I2C, pins, etc.). Return true on success. */
bool sensor_sht40_init(void);

/* Read a single measurement. Returns true on success, false otherwise. */
bool sensor_sht40_read(sensor_sht40_data_t* out);

/* Optional: put sensor into low-power or free resources. */
void sensor_sht40_deinit(void);

#ifdef __cplusplus
}
#endif

#endif /* SENSOR_SHT40_H */
