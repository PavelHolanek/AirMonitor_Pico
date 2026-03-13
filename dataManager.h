#ifndef DATA_MANAGER_H
#define DATA_MANAGER_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "Clock.h"
#include "sensors/sensor_bmp280.h"
#include "sensors/sensor_sht40.h"
#include "sensors/sensor_sdc41.h"

#ifdef __cplusplus
extern "C" {
#endif

#define DATA_MANAGER_BUFFER_CAPACITY 256U

typedef struct {
    Time time;
    sensor_bmp280_data_t bmp280;
    sensor_sht40_data_t sht40;
    sensor_sdc41_data_t sdc41;
} data_manager_entry_t;

typedef struct {
    int32_t temperature_c;
    int32_t humidity_rh;
    int32_t pressure_pa;
    uint16_t co2_ppm;
} data_manager_processed_sample_t;

void dataManager_init(void);

void dataManager_storeSample(
    Time time,
    const sensor_bmp280_data_t* bmp280,
    const sensor_sht40_data_t* sht40,
    const sensor_sdc41_data_t* sdc41);

void dataManager_processSample(
    const sensor_bmp280_data_t* bmp280,
    const sensor_sht40_data_t* sht40,
    const sensor_sdc41_data_t* sdc41,
    data_manager_processed_sample_t* out_processed);

size_t dataManager_count(void);
bool dataManager_getLatest(data_manager_entry_t* out_entry);
bool dataManager_getByAge(size_t age_from_latest, data_manager_entry_t* out_entry);

#ifdef __cplusplus
}
#endif

#endif // DATA_MANAGER_H
