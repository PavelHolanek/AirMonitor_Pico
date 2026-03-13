#include "dataManager.h"
#include <string.h>

typedef struct {
    data_manager_entry_t entries[DATA_MANAGER_BUFFER_CAPACITY];
    size_t head;
    size_t count;
} data_manager_buffer_t;

static data_manager_buffer_t g_dataBuffer;
static int32_t g_temperatureBuffer[DATA_MANAGER_BUFFER_CAPACITY];
static int32_t g_humidityBuffer[DATA_MANAGER_BUFFER_CAPACITY];
static int32_t g_pressureBuffer[DATA_MANAGER_BUFFER_CAPACITY];
static uint16_t g_co2Buffer[DATA_MANAGER_BUFFER_CAPACITY];
static size_t g_processedHead;
static size_t g_processedCount;

void dataManager_init(void)
{
    memset(&g_dataBuffer, 0, sizeof(g_dataBuffer));
    memset(g_temperatureBuffer, 0, sizeof(g_temperatureBuffer));
    memset(g_humidityBuffer, 0, sizeof(g_humidityBuffer));
    memset(g_pressureBuffer, 0, sizeof(g_pressureBuffer));
    memset(g_co2Buffer, 0, sizeof(g_co2Buffer));
    g_processedHead = 0U;
    g_processedCount = 0U;
}

void dataManager_storeSample(
    Time time,
    const sensor_bmp280_data_t* bmp280,
    const sensor_sht40_data_t* sht40,
    const sensor_sdc41_data_t* sdc41)
{
    data_manager_entry_t* slot = &g_dataBuffer.entries[g_dataBuffer.head];
    slot->time = time;

    if (bmp280 != NULL) {
        slot->bmp280 = *bmp280;
    } else {
        memset(&slot->bmp280, 0, sizeof(slot->bmp280));
    }

    if (sht40 != NULL) {
        slot->sht40 = *sht40;
    } else {
        memset(&slot->sht40, 0, sizeof(slot->sht40));
    }

    if (sdc41 != NULL) {
        slot->sdc41 = *sdc41;
    } else {
        memset(&slot->sdc41, 0, sizeof(slot->sdc41));
    }

    g_dataBuffer.head = (g_dataBuffer.head + 1U) % DATA_MANAGER_BUFFER_CAPACITY;
    if (g_dataBuffer.count < DATA_MANAGER_BUFFER_CAPACITY) {
        g_dataBuffer.count++;
    }
}

void dataManager_processSample(
    const sensor_bmp280_data_t* bmp280,
    const sensor_sht40_data_t* sht40,
    const sensor_sdc41_data_t* sdc41,
    data_manager_processed_sample_t* out_processed)
{
    data_manager_processed_sample_t processed;

    memset(&processed, 0, sizeof(processed));

    if (bmp280 != NULL) {
        processed.temperature_c = bmp280->temperature_c;
        processed.pressure_pa = bmp280->pressure_pa;
    }
    if (sht40 != NULL) {
        processed.humidity_rh = sht40->humidity_rh;
    }
    if (sdc41 != NULL) {
        processed.co2_ppm = sdc41->co2_ppm;
    }

    g_temperatureBuffer[g_processedHead] = processed.temperature_c;
    g_humidityBuffer[g_processedHead] = processed.humidity_rh;
    g_pressureBuffer[g_processedHead] = processed.pressure_pa;
    g_co2Buffer[g_processedHead] = processed.co2_ppm;

    g_processedHead = (g_processedHead + 1U) % DATA_MANAGER_BUFFER_CAPACITY;
    if (g_processedCount < DATA_MANAGER_BUFFER_CAPACITY) {
        g_processedCount++;
    }

    if (out_processed != NULL) {
        *out_processed = processed;
    }
}

size_t dataManager_count(void)
{
    return g_dataBuffer.count;
}

bool dataManager_getLatest(data_manager_entry_t* out_entry)
{
    return dataManager_getByAge(0U, out_entry);
}

bool dataManager_getByAge(size_t age_from_latest, data_manager_entry_t* out_entry)
{
    size_t idx;

    if (out_entry == NULL || g_dataBuffer.count == 0U || age_from_latest >= g_dataBuffer.count) {
        return false;
    }

    idx = (g_dataBuffer.head + DATA_MANAGER_BUFFER_CAPACITY - 1U - age_from_latest)
          % DATA_MANAGER_BUFFER_CAPACITY;
    *out_entry = g_dataBuffer.entries[idx];
    return true;
}
