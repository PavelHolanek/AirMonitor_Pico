/*
 * Sensor driver skeleton implementation for SDC41
 * Replace with actual implementation as needed.
 */
#include "sensor_sdc41.h"

#include "scd4x_i2c.h"
#include "sensirion_common.h"
#include "sensirion_i2c_hal.h"
#include <inttypes.h>
#include <stddef.h>

#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/spi.h"

uint16_t co2_concentration = 0;
int32_t temperature = 0;
int32_t relative_humidity = 0;

void convert_and_print_serial(uint16_t* serial_raw) {
    uint64_t serial_as_int = 0;
    sensirion_common_to_integer((uint8_t*)serial_raw, (uint8_t*)&serial_as_int,
                                LONG_INTEGER, 6);
    printf("0x%" PRIx64, serial_as_int);
}

bool sensor_sdc41_init(void) {
    int16_t error = NO_ERROR;
    sensirion_i2c_hal_init();
    scd4x_init(SCD41_I2C_ADDR_62);

    uint16_t serial_number[3] = {0};
    sleep_ms(300);
    // Ensure sensor is in clean state
    error = scd4x_wake_up();
    if (error != NO_ERROR) {
        printf("error executing wake_up(): %i\n", error);
        return false;
    }

    error = scd4x_stop_periodic_measurement();
    if (error != NO_ERROR) {
        printf("error executing stop_periodic_measurement(): %i\n", error);
        return false;
    }

    error = scd4x_reinit();
    if (error != NO_ERROR) {
        printf("error executing reinit(): %i\n", error);
        return false;
    }

    // Read out information about the sensor
    error = scd4x_get_serial_number(serial_number, 3);
    if (error != NO_ERROR) {
        printf("error executing get_serial_number(): %i\n", error);
        return false;
    }
    printf("serial number: ");
    convert_and_print_serial(serial_number);
    printf("\n");
    //
    // If temperature offset and/or sensor altitude compensation
    // is required, you should call the respective functions here.
    // Check out the header file for the function definitions.
    // Start periodic measurements (5sec interval)
    error = scd4x_start_periodic_measurement();
    if (error != NO_ERROR) {
        printf("error executing start_periodic_measurement(): %i\n", error);
        return false;
    }
    //
    // If low-power mode is required, switch to the low power
    // measurement function instead of the standard measurement
    // function above. Check out the header file for the definition.
    //
    return true;
}

bool sensor_sdc41_read(sensor_sdc41_data_t* out) {
    if (!out) return false;

    bool data_ready = false;
    int16_t error = NO_ERROR;

    error = scd4x_get_data_ready_status(&data_ready);
    if (error != NO_ERROR) {
        printf("error executing get_data_ready_status(): %i\n", error);
        return false;
    }

    while (!data_ready)
    {
        sleep_ms(1000);
        error = scd4x_get_data_ready_status(&data_ready);
        if (error != NO_ERROR)
        {
            printf("error executing get_data_ready_status(): %i\n", error);
            continue;
        }
    }
    error = scd4x_read_measurement(&co2_concentration, &temperature,
                                    &relative_humidity);
    if (error != NO_ERROR) {
        printf("error executing read_measurement(): %i\n", error);
        return false;
    }

    out->co2_ppm = co2_concentration;
    out->temperature_c = temperature;
    out->humidity_rh = relative_humidity;
    return true;
}

void sensor_sdc41_deinit(void) {
    // TODO: deinitialize sensor / free resources
}
