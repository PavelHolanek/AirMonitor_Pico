#ifndef DATA_MANAGER_H
#define DATA_MANAGER_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "Clock.h"
#include "Base.h"
#include "sensors/sensor_bmp280.h"
#include "sensors/sensor_sht40.h"
#include "sensors/sensor_sdc41.h"

#ifdef __cplusplus
extern "C" {
#endif

// TODO ESP32-S3 migration: with 8 MB PSRAM move the ring buffer there and raise
// this, the RP2350 heap stops being the limit. Worth revisiting at the same
// time: GraphWidget copies the whole ring into a graph_sample_t array; once the
// data lives in PSRAM it should only pull the frame plus the neighbouring
// samples just outside it.
#define DATA_MANAGER_BUFFER_CAPACITY 1024U

typedef struct
{
    int32_t temperature_c;
    int32_t humidity_rh;
    int32_t pressure_pa;
    uint16_t co2_ppm;
    Time time;
} data_manager_processed_sample_t;

int32_t extract_data_for_quantity(data_manager_processed_sample_t* data, QUANTITY quantity);

void dataManager_init(void);

size_t dataManager_count(void);

data_manager_processed_sample_t* dataManager_get_data(size_t index);

void dataManager_store_and_process_sample(
    Time time,
    const sensor_bmp280_data_t* bmp280,
    const sensor_sht40_data_t* sht40,
    const sensor_sdc41_data_t* sdc41);

#ifdef __cplusplus
}
#endif

#endif // DATA_MANAGER_H
