/*
 * Sensor driver skeleton implementation for bmp280
 * Replace with actual implementation as needed.
 */
#include "sensor_bmp280.h"
#include "bmp280.h"
#include "Pinout.h"
#include "sensors_i2c.h"
#include <stddef.h>
#include <stdio.h>

int32_t raw_temperature = 0;
int32_t raw_pressure = 0;
struct bmp280_calib_param params;

bool sensor_bmp280_init(void)
{
    // useful information for picotool
    bi_decl(bi_2pins_with_func(SENSORS_I2C_SDA_PIN, SENSORS_I2C_SCL_PIN, GPIO_FUNC_I2C));
    bi_decl(bi_program_description("BMP280 I2C example for the Raspberry Pi Pico"));

    bmp280_setPins(SENSORS_I2C_SDA_PIN, SENSORS_I2C_SCL_PIN, sensors_i2c_init());

    bmp280_init();
    bmp280_get_calib_params(&params);
    sleep_ms(250);
    return true;
}

bool sensor_bmp280_read(sensor_bmp280_data_t* out)
{
    if (!out) return false;

    bmp280_read_raw(&raw_temperature, &raw_pressure);
    int32_t temperature = bmp280_convert_temp(raw_temperature, &params);
    int32_t pressure = bmp280_convert_pressure(raw_pressure, raw_temperature, &params);

    out->temperature_c = temperature;
    out->humidity_rh = 0;
    out->pressure_pa = pressure;
    return true; // stub: indicate no real data yet
}

void sensor_bmp280_deinit(void)
{
    // TODO: deinitialize sensor / free resources
}
