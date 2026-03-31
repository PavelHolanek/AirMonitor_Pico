#include "dataManager.h"
#include <string.h>

static data_manager_entry_t raw_data[DATA_MANAGER_BUFFER_CAPACITY];
static data_manager_processed_sample_t processed_data[DATA_MANAGER_BUFFER_CAPACITY];

static size_t data_count;
static size_t data_beginning_index;

void dataManager_init(void)
{
    memset(raw_data, 0, sizeof(raw_data));
    memset(processed_data, 0, sizeof(processed_data));
    data_count = 0;
    data_beginning_index = 0;
}

size_t dataManager_count(void)
{
    return data_count;
}

void dataManager_store_and_process_sample(
    Time time,
    const sensor_bmp280_data_t* bmp280,
    const sensor_sht40_data_t* sht40,
    const sensor_sdc41_data_t* sdc41)
{
    size_t index;
    if (data_count < DATA_MANAGER_BUFFER_CAPACITY)
    {
        index = ++data_count - 1;
    }
    else
    {
        index = data_beginning_index;
        data_beginning_index = (data_beginning_index + 1) % DATA_MANAGER_BUFFER_CAPACITY;
    }

    data_manager_entry_t* raw = &(raw_data[index]);
    data_manager_processed_sample_t* processed = &(processed_data[index]);

    raw->bmp280 = *bmp280;
    raw->sht40 = *sht40;
    raw->sdc41 = *sdc41;
    raw->time = time;

    //better algoriths will be implemented
    processed->temperature_c = bmp280->temperature_c;
    processed->pressure_pa = bmp280->pressure_pa;
    processed->humidity_rh = sht40->humidity_rh;
    processed->co2_ppm = sdc41->co2_ppm;
    processed->time = time;
}

data_manager_processed_sample_t* dataManager_get_data(size_t index)
{
    if (index >= data_count)
    {
        return NULL;
    }
    return &(processed_data[(data_beginning_index + index) % DATA_MANAGER_BUFFER_CAPACITY]);
}

int32_t extract_data_for_quantity(data_manager_processed_sample_t* data, QUANTITY quantity)
{
    switch (quantity)
    {
    case QUANTITY_CO2:
        return data->co2_ppm;
    case QUANTITY_HUMIDITY:
        return data->humidity_rh;
    case QUANTITY_PRESSURE:
        return data->pressure_pa;
    case QUANTITY_TEMPERATURE:
        return data->temperature_c;
    default:
        break;
    }
}

