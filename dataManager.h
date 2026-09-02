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

// At a 2 min measurement period this is 5 d 16 h of history, enough to fill the
// widest graph scope (3 d 8 h) with room to spare.
//
// Cost: capacity * (sizeof(data_manager_entry_t) + sizeof(data_manager_processed_sample_t))
//     = 4096 * (40 + 20) = 240 KB
// plus capacity * sizeof(graph_sample_t) = 4096 * 12 = 48 KB for the copy that
// GraphWidget hands to the graph algorithm. That is 288 KB of the 512 KB SRAM
// on the RP2350, which leaves roughly 84 KB free.
//
// TODO ESP32-S3 migration: this value is capped by RP2350 SRAM, nothing else.
// With 8 MB PSRAM move both ring buffers there and raise it - 8192 samples
// (11 d 9 h) cost 480 KB + 96 KB and simply do not fit here. Two things worth
// revisiting at the same time:
//   - raw_data is currently write-only dead weight (2/3 of the cost above);
//     either drop it or keep only a short window for the processing filter
//   - GraphWidget copies the whole ring into a graph_sample_t array; once the
//     data lives in PSRAM it should only pull the frame plus the neighbouring
//     samples just outside it
#define DATA_MANAGER_BUFFER_CAPACITY 4096U

typedef struct
{
    Time time;
    sensor_bmp280_data_t bmp280;
    sensor_sht40_data_t sht40;
    sensor_sdc41_data_t sdc41;
} data_manager_entry_t;

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
