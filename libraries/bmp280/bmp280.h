#ifndef BMP280_H
#define BMP280_H

#include "hardware/i2c.h"
#include "pico/binary_info.h"
#include "pico/stdlib.h"

#define BMP280_I2C_SDA_PIN 20
#define BMP280_I2C_SCL_PIN 21
#define BMP280_I2C &i2c0_inst

struct bmp280_calib_param {
    // temperature params
    uint16_t dig_t1;
    int16_t dig_t2;
    int16_t dig_t3;

    // pressure params
    uint16_t dig_p1;
    int16_t dig_p2;
    int16_t dig_p3;
    int16_t dig_p4;
    int16_t dig_p5;
    int16_t dig_p6;
    int16_t dig_p7;
    int16_t dig_p8;
    int16_t dig_p9;
};

void bmp280_init();
void bmp280_read_raw(int32_t* temp, int32_t* pressure);
void bmp280_reset();
int32_t bmp280_convert(int32_t temp, struct bmp280_calib_param* params);
int32_t bmp280_convert_temp(int32_t temp, struct bmp280_calib_param* params);
int32_t bmp280_convert_pressure(int32_t pressure, int32_t temp, struct bmp280_calib_param* params);
void bmp280_get_calib_params(struct bmp280_calib_param* params);

#endif // BMP280_H