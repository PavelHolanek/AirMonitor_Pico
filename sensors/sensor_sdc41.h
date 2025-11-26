/*
 * Sensor driver skeleton for SDC41
 * This is a stub interface. Replace with actual implementation as needed.
 */
#ifndef SENSOR_SDC41_H
#define SENSOR_SDC41_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>

/* Basic measurement set for SDC41 (CO2 + T + RH) */
typedef struct {
    float co2_ppm;
    float temperature_c;
    float humidity_rh;
} sensor_sdc41_data_t;

/* Initialize the sensor (I2C, pins, etc.). Return true on success. */
bool sensor_sdc41_init(void);

/* Read a single measurement. Returns true on success, false otherwise. */
bool sensor_sdc41_read(sensor_sdc41_data_t* out);

/* Optional: put sensor into low-power or free resources. */
void sensor_sdc41_deinit(void);

#ifdef __cplusplus
}
#endif

#endif /* SENSOR_SDC41_H */
