#include "Tasks.h"
#include "Clock.h"
#include "Time.h"
#include "log.h"
#include "Settings.h"
#include "sensors/sensor_bme280.h"
#include "sensors/sensor_sht40.h"
#include "sensors/sensor_sdc41.h"

void intializeSemaphoresAndQueues(void)
{
i2c0_semaphore = xSemaphoreCreateBinary();
i2c1_semaphore = xSemaphoreCreateBinary();
spi0_semaphore = xSemaphoreCreateBinary();
spi1_semaphore = xSemaphoreCreateBinary();

BME280DataQueue = xQueueCreate(1, sizeof(sensor_bme280_data_t));
SHT40DataQueue = xQueueCreate(1, sizeof(sensor_sht40_data_t));
SDC41DataQueue = xQueueCreate(1, sizeof(sensor_sdc41_data_t));

TemperatureQueue = xQueueCreate(1, sizeof(uint16_t));
PreassureQueue = xQueueCreate(1, sizeof(uint16_t));
HumidityQueue = xQueueCreate(1, sizeof(uint16_t));
CO2Queue = xQueueCreate(1, sizeof(uint16_t));

CurrentTimeQueue = xQueueCreate(1, sizeof(Time));

LogsToStoreQueue = xQueueCreate(8, sizeof(char*));

JoystickStateQueue = xQueueCreate(1, sizeof(uint8_t));
}

void getTimeTask(void) {
    // For now, just call getTime(). Synchronization with RTC will be added later.
    Time t = getClockTime();
    (void)t; // suppress unused variable warning for now
}

void getClockTimeTask(void) {
    for(;;) 
    {

    }
}

void readBME280Task(void) {
    sensor_bme280_data_t value;
    for(;;)
    {
        xSemaphoreTake(i2c0_semaphore, TICKS_TO_WAIT);

        if (sensor_bme280_read(&value))
        {
            if(xQueueSend(BME280DataQueue, ( void * ) &value, TICKS_TO_WAIT) != pdPASS )
            {
                LOG("Failed to send BME280 data");
            }
        }
        else
        {
            LOG("Failed to read data from BME280");
        }
        
        xSemaphoreGive(i2c0_semaphore);
        vTaskDelay(pdMS_TO_TICKS(sensorsMeassurementPeriod));
    }
}

void readSHT40Task(void) {
    sensor_sht40_data_t value;
    for(;;)
    { 
        xSemaphoreTake(i2c0_semaphore, TICKS_TO_WAIT);

        if (sensor_sht40_read(&value))
        {
            if(xQueueSend(SHT40DataQueue, ( void * ) &value, TICKS_TO_WAIT) != pdPASS )
            {
                LOG("Failed to send SHT40 data");
            }
        }
        else
        {
            LOG("Failed to read data from SHT40");
        }
        
        xSemaphoreGive(i2c0_semaphore);
        vTaskDelay(pdMS_TO_TICKS(sensorsMeassurementPeriod));
    }
}

void readSCD41Task(void) {
    sensor_sdc41_data_t value;
    for(;;)
    { 
        xSemaphoreTake(i2c0_semaphore, TICKS_TO_WAIT);

        if (sensor_sdc41_read(&value))
        {
            if(xQueueSend(SDC41DataQueue, ( void * ) &value, TICKS_TO_WAIT) != pdPASS )
            {
                LOG("Failed to send SDC41 data");
            }
        }
        else
        {
            LOG("Failed to read data from SDC41");
        }
        
        xSemaphoreGive(i2c0_semaphore);
        vTaskDelay(pdMS_TO_TICKS(sensorsMeassurementPeriod));
    }
}

void dataManagerTask(void) {
    sensor_sdc41_data_t sdc41value;
    sensor_sht40_data_t sht40value;
    sensor_bme280_data_t bme280value;

    u_int16_t temperature;
    u_int16_t humidity;
    u_int16_t preassure;
    u_int16_t co2;

    for(;;)
    { 
        xQueueReceive(SDC41DataQueue, &sdc41value, TICKS_TO_WAIT);
        xQueueReceive(SHT40DataQueue, &sht40value, TICKS_TO_WAIT);
        xQueueReceive(BME280DataQueue, &bme280value, TICKS_TO_WAIT);

        //TODO moving average
        temperature = (bme280value.pressure_pa + sht40value.temperature_c) / 2;
        humidity = sht40value.humidity_rh;
        preassure = bme280value.pressure_pa;
        co2 = sdc41value.co2_ppm;

        //TODO field for graph
        //TODO value trend

        xQueueSend(TemperatureQueue, (void*) &temperature, TICKS_TO_WAIT);
        xQueueSend(PreassureQueue, (void*) &preassure, TICKS_TO_WAIT);
        xQueueSend(HumidityQueue, (void*) &humidity, TICKS_TO_WAIT);
        xQueueSend(CO2Queue, (void*) &co2, TICKS_TO_WAIT);
    }
}

void valuesChangedGUITask(void) {
    u_int16_t temperature;
    u_int16_t humidity;
    u_int16_t preassure;
    u_int16_t co2;
    for(;;)
    { 
        xQueueReceive(TemperatureQueue, &temperature, TICKS_TO_WAIT);
        xQueueReceive(PreassureQueue, &preassure, TICKS_TO_WAIT);
        xQueueReceive(HumidityQueue, &humidity, TICKS_TO_WAIT);
        xQueueReceive(CO2Queue, &co2, TICKS_TO_WAIT);

        //TODO update of GUI
    }
}

void timeChangedGUITask(void) {
    for(;;)
    { 

    }
}

void joystickEvaluationTask(void) {
    for(;;)
    { 

    }
}

void joystickActionGUITask(void) {
    for(;;)
    { 

    }
}

void writeLogTask(void) {
    for(;;)
    { 

    }
}

void writeValueToStorageTask(void) {
    for(;;)
    { 

    }
}
