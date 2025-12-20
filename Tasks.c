#include "Tasks.h"
#include "Clock.h"
#include "Log.h"
#include "Settings.h"
#include "Base.h"
#include "GUIManager.c"
#include "sensors/sensor_bmp280.h"
#include "sensors/sensor_sht40.h"
#include "sensors/sensor_sdc41.h"
#include <stdint.h>
#include <stdio.h>
#include "Libraries/pico-displayDrivs/gfx/gfx.h"
#include "hardware/adc.h"

SemaphoreHandle_t i2c0_mutex = NULL;
SemaphoreHandle_t i2c1_mutex = NULL;
SemaphoreHandle_t spi0_mutex = NULL;
SemaphoreHandle_t spi1_mutex = NULL;

QueueHandle_t bmp280DataQueue = NULL;
QueueHandle_t SHT40DataQueue = NULL;
QueueHandle_t SDC41DataQueue = NULL;

QueueHandle_t TemperatureQueue = NULL;
QueueHandle_t PreassureQueue = NULL;
QueueHandle_t HumidityQueue = NULL;
QueueHandle_t CO2Queue = NULL;

SemaphoreHandle_t TimeRequestSemaphore = NULL;
QueueHandle_t CurrentTimeQueue = NULL;

QueueHandle_t LogsToStoreQueue = NULL;

SemaphoreHandle_t JoystickMoveInteruptionSemaphore = NULL;
SemaphoreHandle_t JoystickPressInteruptionSemaphore = NULL;
QueueHandle_t JoystickStateQueue = NULL;

EventGroupHandle_t JoystickEventGroup = NULL;
#define EVENT_FLAG_PRESSED   (1 << 0)
#define EVENT_FLAG_MOVED     (1 << 1)

void intializeSemaphoresAndQueues()
{
    i2c0_mutex = xSemaphoreCreateMutex();
    i2c1_mutex = xSemaphoreCreateMutex();
    spi0_mutex = xSemaphoreCreateMutex();
    spi1_mutex = xSemaphoreCreateMutex();

    bmp280DataQueue = xQueueCreate(1, sizeof(sensor_bmp280_data_t));
    SHT40DataQueue = xQueueCreate(1, sizeof(sensor_sht40_data_t));
    SDC41DataQueue = xQueueCreate(1, sizeof(sensor_sdc41_data_t));

    TemperatureQueue = xQueueCreate(1, sizeof(int32_t));
    PreassureQueue = xQueueCreate(1, sizeof(int32_t));
    HumidityQueue = xQueueCreate(1, sizeof(int32_t));
    CO2Queue = xQueueCreate(1, sizeof(uint16_t));

    TimeRequestSemaphore  = xSemaphoreCreateBinary();
    CurrentTimeQueue = xQueueCreate(1, sizeof(Time));

    LogsToStoreQueue = xQueueCreate(8, sizeof(char*));

    JoystickMoveInteruptionSemaphore = xSemaphoreCreateBinary();
    JoystickPressInteruptionSemaphore = xSemaphoreCreateBinary();
    JoystickStateQueue = xQueueCreate(1, sizeof(uint8_t));

    JoystickEventGroup = xEventGroupCreate();

    sensorsMeassurementPeriod = 30000;
    timeUpdatePeriod = 30000;
}
void getClockTimeTask(void*) {
    Time value;
    for(;;) 
    {
        xSemaphoreTake(TimeRequestSemaphore, portMAX_DELAY);
        printf("CLOCK: getClockTimeTask \n");
        value = getClockTimeImpl();
        xQueueOverwrite(CurrentTimeQueue, ( void * ) &value);
    }
}

void readbmp280Task(void*) {
    sensor_bmp280_data_t value;
    for(;;)
    {
        xSemaphoreTake(i2c0_mutex, portMAX_DELAY);
        LOG("TASK: readbmp280");
        if (sensor_bmp280_read(&value))
        {
            if(xQueueSend(bmp280DataQueue, ( void * ) &value, TICKS_TO_WAIT) != pdPASS )
            {
                LOG("Failed to send bmp280 data");
            }
        }
        else
        {
            LOG("Failed to read data from bmp280");
        }
        
        xSemaphoreGive(i2c0_mutex);
        vTaskDelay(pdMS_TO_TICKS(sensorsMeassurementPeriod));
    }
}

void readSHT40Task(void*) {
    sensor_sht40_data_t value;
    for(;;)
    { 
        xSemaphoreTake(i2c0_mutex, portMAX_DELAY);
        LOG("TASK: readSHT40");
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
        
        xSemaphoreGive(i2c0_mutex);
        vTaskDelay(pdMS_TO_TICKS(sensorsMeassurementPeriod));
    }
}

void readSCD41Task(void*) {
    sensor_sdc41_data_t value;
    for(;;)
    { 
        xSemaphoreTake(i2c0_mutex, portMAX_DELAY);
        LOG("TASK: readSCD41");
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
        
        xSemaphoreGive(i2c0_mutex);
        vTaskDelay(pdMS_TO_TICKS(sensorsMeassurementPeriod));
    }
}

void dataManagerTask(void*) {
    sensor_sdc41_data_t sdc41value;
    sensor_sht40_data_t sht40value;
    sensor_bmp280_data_t bmp280value;

    int32_t temperature;
    int32_t humidity;
    int32_t preassure;
    uint16_t co2;

    for(;;)
    { 
        xQueueReceive(SDC41DataQueue, &sdc41value, portMAX_DELAY);
        xQueueReceive(SHT40DataQueue, &sht40value, portMAX_DELAY);
        xQueueReceive(bmp280DataQueue, &bmp280value, portMAX_DELAY);
        LOG("TASK: dataManager");
        //TODO moving average
        temperature = bmp280value.temperature_c;
        humidity = sht40value.humidity_rh;
        preassure = bmp280value.pressure_pa;
        co2 = sdc41value.co2_ppm;

        //TODO field for graph
        //TODO value trend

        xQueueSend(TemperatureQueue, (void*) &temperature, TICKS_TO_WAIT);
        xQueueSend(PreassureQueue, (void*) &preassure, TICKS_TO_WAIT);
        xQueueSend(HumidityQueue, (void*) &humidity, TICKS_TO_WAIT);
        xQueueSend(CO2Queue, (void*) &co2, TICKS_TO_WAIT);
    }
}

void valuesChangedGUITask(void*) {
    int32_t temperature;
    int32_t humidity;
    int32_t preassure;
    uint16_t co2;
    for(;;)
    { 
        xQueueReceive(TemperatureQueue, &temperature, portMAX_DELAY);
        xQueueReceive(PreassureQueue, &preassure, portMAX_DELAY);
        xQueueReceive(HumidityQueue, &humidity, portMAX_DELAY);
        xQueueReceive(CO2Queue, &co2, portMAX_DELAY);
        xSemaphoreTake(spi0_mutex, portMAX_DELAY);

        LOG("TASK: valuesChangedGUI");

        gui_dataChanged(QUANTITY_TEMPERATURE, temperature);
        gui_dataChanged(QUANTITY_PRESSURE, preassure);
        gui_dataChanged(QUANTITY_HUMIDITY, humidity);
        gui_dataChanged(QUANTITY_CO2, co2);
        xSemaphoreGive(spi0_mutex);
    }
}

void timeChangedGUITask(void*) {
    Time currentTime;
    for(;;)
    {
        xSemaphoreTake(spi0_mutex, portMAX_DELAY);
        LOG("TASK: timeChangedGUI");
        currentTime = getClockTime();
        gui_timeChanged(currentTime);
        xSemaphoreGive(spi0_mutex);
        vTaskDelay(pdMS_TO_TICKS(timeUpdatePeriod));
    }
}

void joystickPressedTask(void*) {
    for(;;)
    { 
        xSemaphoreTake(JoystickPressInteruptionSemaphore, portMAX_DELAY);
        LOG("TASK: joystickPressed");
        xEventGroupSetBits(JoystickEventGroup, EVENT_FLAG_PRESSED);
        vTaskDelay(JOYSTICK_DEBOUNCING_PERIOD);
        xSemaphoreTake(JoystickPressInteruptionSemaphore, 0);
    }
}

void joystickMovedTask(void*) {
    for(;;)
    { 
        xSemaphoreTake(JoystickMoveInteruptionSemaphore, portMAX_DELAY);
        LOG("TASK: joystickMoved");
        xEventGroupSetBits(JoystickEventGroup, EVENT_FLAG_MOVED);
        vTaskDelay(JOYSTICK_DEBOUNCING_PERIOD);
        xSemaphoreTake(JoystickMoveInteruptionSemaphore,0);
    }
}

void joystickEvaluationTask(void*) {
    adc_init();
    adc_gpio_init(26);
    adc_gpio_init(27);
    struct JoystickState state;
    for(;;)
    { 
        state.horizontal = 0;
        state.vertical = 0;
        EventBits_t uxBits = xEventGroupWaitBits(JoystickEventGroup,
                                                EVENT_FLAG_PRESSED | EVENT_FLAG_MOVED,
                                                pdTRUE,
                                                pdFALSE,
                                                portMAX_DELAY);
        xSemaphoreTake(spi0_mutex, portMAX_DELAY);
        LOG("TASK: joystickEvaluation");
        state.pressed = ((uxBits & EVENT_FLAG_PRESSED) != 0);
        if ((uxBits & EVENT_FLAG_MOVED) != 0) 
        {
            adc_select_input(0);
            state.horizontal = adc_read() - 2065;
            adc_select_input(1);
            state.vertical = adc_read() - 2000;
        }
        gui_joystick(state);
        xSemaphoreGive(spi0_mutex);
    }
}

void writeLogTask(void*) {
    for(;;)
    { 
        LOG("TASK: writeLog");
    }
}

void writeValueToStorageTask(void*) {
    for(;;)
    { 
        LOG("TASK: writeValueToStorage");
    }
}
