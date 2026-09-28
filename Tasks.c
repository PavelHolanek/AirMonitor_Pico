#include "Tasks.h"
#include "Clock.h"
#include "Log.h"
#include "Settings.h"
#include "Base.h"
#include "dataManager.h"
#include "GUIManager_c.h"
#include "sensors/sensor_bmp280.h"
#include "sensors/sensor_sht40.h"
#include "sensors/sensor_sdc41.h"
#include <stdint.h>
#include <stdio.h>
#include "Libraries/pico-displayDrivs/gfx/gfx.h"
#include "Pinout.h"

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

SemaphoreHandle_t TimeSetRequestSemaphore = NULL;
QueueHandle_t TimeToSetQueue = NULL;

QueueHandle_t LogsToStoreQueue = NULL;

QueueHandle_t GuiEventQueue = NULL;

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

    TimeSetRequestSemaphore  = xSemaphoreCreateBinary();
    TimeToSetQueue = xQueueCreate(1, sizeof(Time));

    LogsToStoreQueue = xQueueCreate(8, sizeof(char*));

    GuiEventQueue = xQueueCreate(GUI_EVENT_QUEUE_LENGTH, sizeof(GuiEvent));

    dataManager_init();
}
void setClockTimeTask(void*)
{
    Time value;
    for(;;)
    {
        xSemaphoreTake(TimeSetRequestSemaphore, portMAX_DELAY);
        printf("CLOCK: setClockTimeTask \n");

        // Without a new time there is nothing to write, repaint or compare the
        // history against - value would be whatever was left on the stack.
        if (xQueueReceive(TimeToSetQueue, &value, TICKS_TO_WAIT) != pdPASS)
        {
            continue;
        }

        xSemaphoreTake(spi0_mutex, portMAX_DELAY);
        xSemaphoreTake(i2c1_mutex, portMAX_DELAY);
        setClockTimeImpl(value);
        xSemaphoreGive(i2c1_mutex);

        // Under spi0_mutex: the GUI reads the sample buffer while it redraws, so
        // the history has to go before anything paints from it. The new clock
        // time goes in first as well, otherwise the graph would redraw itself
        // around the time it is about to leave behind.
        const bool erased = dataManager_erase_if_time_jumped(value);
        gui_timeChanged(value);
        if (erased)
        {
            LOG("DATA: history erased, clock moved too far");
            gui_dataChanged();
        }
        xSemaphoreGive(spi0_mutex);
    }
}

void readbmp280Task(void*)
{
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
        vTaskDelay(pdMS_TO_TICKS(intervalToMilliseconds(meassurementInterval)));
    }
}

void readSHT40Task(void*)
{
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
        vTaskDelay(pdMS_TO_TICKS(intervalToMilliseconds(meassurementInterval)));
    }
}

void readSCD41Task(void*)
{
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
        vTaskDelay(pdMS_TO_TICKS(intervalToMilliseconds(meassurementInterval)));
    }
}

void dataManagerTask(void*)
{
    sensor_sdc41_data_t sdc41value;
    sensor_sht40_data_t sht40value;
    sensor_bmp280_data_t bmp280value;
    Time timestamp;
    data_manager_processed_sample_t* processed;

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

        xSemaphoreTake(spi0_mutex, portMAX_DELAY);
        timestamp = getClockTime();
        dataManager_store_and_process_sample(timestamp, &bmp280value, &sht40value, &sdc41value);
        gui_dataChanged();
        xSemaphoreGive(spi0_mutex);
    }
}


void timeChangedGUITask(void*)
{
    Time currentTime;
    for(;;)
    {
        LOG("TASK: timeChangedGUI");
        currentTime = getClockTime();
        xSemaphoreTake(spi0_mutex, portMAX_DELAY);
        gui_timeChanged(currentTime);
        xSemaphoreGive(spi0_mutex);
        vTaskDelay(pdMS_TO_TICKS(timeUpdatePeriod));
    }
}

void postJoystickEventToGui(const JoystickEvent* event)
{
    GuiEvent guiEvent;
    guiEvent.type = GUI_EVENT_JOYSTICK;
    guiEvent.joystick = *event;
    // Runs on the joystick task, which must never wait for the GUI.
    if (xQueueSend(GuiEventQueue, &guiEvent, 0) != pdPASS)
    {
        LOG("GUI: event queue full, joystick event dropped");
    }
}

void guiTask(void*)
{
    GuiEvent event;
    for(;;)
    {
        // Sleep until an event comes or the nearest GUI timer is due
        const uint32_t waitMs = gui_msUntilNextTimer();
        const TickType_t wait = (waitMs == UINT32_MAX) ? portMAX_DELAY : pdMS_TO_TICKS(waitMs);
        const bool received = (xQueueReceive(GuiEventQueue, &event, wait) == pdPASS);

        xSemaphoreTake(spi0_mutex, portMAX_DELAY);
        if (received)
        {
            gui_handleEvent(&event);
        }
        // After an event as well, so a stream of events cannot starve the timers
        gui_processTimers();
        xSemaphoreGive(spi0_mutex);
    }
}

void writeLogTask(void*)
{
    for(;;)
    { 
        LOG("TASK: writeLog");
    }
}

void writeValueToStorageTask(void*)
{
    for(;;)
    { 
        LOG("TASK: writeValueToStorage");
    }
}
