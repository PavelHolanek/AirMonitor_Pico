/*
 * Pico implementation of the Sensirion I2C HAL.
 *
 * Both Sensirion submodules ship this file as a stub full of TODO:IMPLEMENT -
 * it is the one file the integrator is expected to write. It lives here rather
 * than in either submodule so that the submodules stay pristine and there is
 * exactly one definition of these symbols in the build. The SHT40 and the
 * SCD41 share it, which is why this sits in sensors/sensirion/ and not under a
 * per sensor folder.
 *
 * The bus itself belongs to sensors_i2c.c, because the BMP280 is on it too.
 * This file only borrows it.
 */

#include "hardware/i2c.h"
#include "pico/stdlib.h"

#include "sensirion_common.h"
#include "sensirion_config.h"
#include "sensirion_i2c_hal.h"

#include "sensors_i2c.h"

// Handed over by sensors_i2c_init(). NULL until sensirion_i2c_hal_init() runs,
// so the transfer functions check it rather than passing NULL into the SDK.
static i2c_inst_t* sensirion_i2c = NULL;

/**
 * Select the current i2c bus by index.
 * All following i2c operations will be directed at that bus.
 *
 * THE IMPLEMENTATION IS OPTIONAL ON SINGLE-BUS SETUPS (all sensors on the same
 * bus)
 *
 * @param bus_idx   Bus index to select
 * @returns         0 on success, an error code otherwise
 */
int16_t sensirion_i2c_hal_select_bus(uint8_t bus_idx) {
    /* TODO:IMPLEMENT or leave empty if all sensors are located on one single
     * bus
     */
    return NOT_IMPLEMENTED_ERROR;
}

/**
 * Initialize all hard- and software components that are needed for the I2C
 * communication.
 */
void sensirion_i2c_hal_init(void) {
    sensirion_i2c = sensors_i2c_init();
}

/**
 * Release all resources initialized by sensirion_i2c_hal_init().
 */
void sensirion_i2c_hal_free(void) {
    // Deliberately does not tear the bus down. The BMP280 sits on the same two
    // pins, so deinitializing the peripheral and dropping the pull ups here
    // would take the bus away from a sensor that is still using it. This HAL
    // does not own the bus, it only borrowed it.
    sensirion_i2c = NULL;
}

/**
 * Execute one read transaction on the I2C bus, reading a given number of bytes.
 * If the device does not acknowledge the read command, an error shall be
 * returned.
 *
 * @param address 7-bit I2C address to read from
 * @param data    pointer to the buffer where the data is to be stored
 * @param count   number of bytes to read from I2C and store in the buffer
 * @returns 0 on success, error code otherwise
 */
int8_t sensirion_i2c_hal_read(uint8_t address, uint8_t* data, uint8_t count) {
    if (sensirion_i2c == NULL)
        return 1;

    int status = i2c_read_blocking(sensirion_i2c, address, data, count, false);

    if (status <= 0)
        return 1;
    else
        return 0;
}

/**
 * Execute one write transaction on the I2C bus, sending a given number of
 * bytes. The bytes in the supplied buffer must be sent to the given address. If
 * the slave device does not acknowledge any of the bytes, an error shall be
 * returned.
 *
 * @param address 7-bit I2C address to write to
 * @param data    pointer to the buffer containing the data to write
 * @param count   number of bytes to read from the buffer and send over I2C
 * @returns 0 on success, error code otherwise
 */
int8_t sensirion_i2c_hal_write(uint8_t address, const uint8_t* data,
                               uint8_t count) {
    if (sensirion_i2c == NULL)
        return 1;

    int status = i2c_write_blocking(sensirion_i2c, address, data, count, false);

    if (status <= 0)
        return 1;
    else
        return 0;
}

/**
 * Sleep for a given number of microseconds. The function should delay the
 * execution for at least the given time, but may also sleep longer.
 *
 * Despite the unit, the resolution is a whole millisecond. Every call site in
 * both drivers passes a whole number of milliseconds, so nothing is lost today,
 * but a sub-millisecond request would round down to no delay at all.
 *
 * @param useconds the sleep time in microseconds
 */
void sensirion_i2c_hal_sleep_usec(uint32_t useconds) {
    sleep_ms(useconds / 1000);
}
