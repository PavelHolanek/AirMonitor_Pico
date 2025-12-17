/*
 * Sensor driver skeleton implementation for SHT40
 * Replace with actual implementation as needed.
 */
#include "sensor_sht40.h"
#include "sensirion_common.h"
#include "sensirion_i2c_hal.h"
#include "sht4x_i2c.h"
#include <inttypes.h>  // PRIx64
#include <stdio.h>     // printf
#include "pico/stdlib.h"

#include <stddef.h>

int32_t temperature_milli_degC = 0;
int32_t humidity_milli_RH = 0;

bool sensor_sht40_init(void) {
    int16_t error = NO_ERROR;
    //sensirion_i2c_hal_init(); already done by BMP280
    sht4x_init(SHT40_I2C_ADDR_44);

    sht4x_soft_reset();
    sleep_ms(10);
    uint32_t serial_number = 0;
    error = sht4x_serial_number(&serial_number);
    if (error != NO_ERROR) {
        printf("error executing serial_number(): %i\n", error);
        return false;
    }
    printf("serial_number: %u\n", serial_number);
    return true;
}

bool sensor_sht40_read(sensor_sht40_data_t* out) {
    if (!out) return false;
    int16_t error = NO_ERROR;
    sleep_ms(20);
        error = sht4x_measure_lowest_precision(&temperature_milli_degC,
                                               &humidity_milli_RH);
        if (error != NO_ERROR) {
            printf("error executing measure_lowest_precision_ticks(): %i\n",
                   error);
            return false;
        }
    out->temperature_c = temperature_milli_degC;
    out->humidity_rh = humidity_milli_RH;
    return true;
}

void sensor_sht40_deinit(void) {
    // TODO: deinitialize sensor / free resources
}
