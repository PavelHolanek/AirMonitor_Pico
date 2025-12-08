/*
 * Sensor driver skeleton implementation for BME280
 * Replace with actual implementation as needed.
 */
#include "sensor_bme280.h"
#include "bmp280.h"
#include <stddef.h>
#include <stdio.h>

int32_t raw_temperature = 0;
int32_t raw_pressure = 0;
struct bmp280_calib_param params;

bool sensor_bme280_init(void) {
    // useful information for picotool
    bi_decl(bi_2pins_with_func(BMP280_I2C_SDA_PIN, BMP280_I2C_SCL_PIN, GPIO_FUNC_I2C));
    bi_decl(bi_program_description("BMP280 I2C example for the Raspberry Pi Pico"));
    // I2C is "open drain", pull ups to keep signal high when no data is being sent
    i2c_init(BMP280_I2C, 100 * 1000);
    gpio_set_function(BMP280_I2C_SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(BMP280_I2C_SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(BMP280_I2C_SDA_PIN);
    gpio_pull_up(BMP280_I2C_SCL_PIN);

    // configure BMP280
    bmp280_init();

    // retrieve fixed compensation params
    bmp280_get_calib_params(&params);
    sleep_ms(250);
    return true; // stub: report initialized
}

bool sensor_bme280_read(sensor_bme280_data_t* out) {
    if (!out) return false;

    bmp280_read_raw(&raw_temperature, &raw_pressure);
    int32_t temperature = bmp280_convert_temp(raw_temperature, &params);
    int32_t pressure = bmp280_convert_pressure(raw_pressure, raw_temperature, &params);
    printf("Pressure = %.3f kPa\n", raw_pressure / 1000.f);
    printf("Temp. = %.2f C\n", raw_temperature / 100.f);

    out->temperature_c = temperature;
    out->humidity_rh = 0;
    out->pressure_pa = pressure;
    return true; // stub: indicate no real data yet
}

void sensor_bme280_deinit(void) {
    // TODO: deinitialize sensor / free resources
}
