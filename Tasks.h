#ifndef TASKS_H
#define TASKS_H

#ifdef __cplusplus
extern "C" {
#endif

#include "Clock.h"
 
#include "FreeRTOS.h"
#include "semphr.h"

#define TICKS_TO_WAIT 1000

SemaphoreHandle_t i2c0_semaphore;
SemaphoreHandle_t i2c1_semaphore;
SemaphoreHandle_t spi0_semaphore;
SemaphoreHandle_t spi1_semaphore;

QueueHandle_t BME280DataQueue;
QueueHandle_t SHT40DataQueue;
QueueHandle_t SDC41DataQueue;

QueueHandle_t TemperatureQueue;
QueueHandle_t PreassureQueue;
QueueHandle_t HumidityQueue;
QueueHandle_t CO2Queue;

QueueHandle_t CurrentTimeQueue;

QueueHandle_t LogsToStoreQueue;

QueueHandle_t JoystickStateQueue;

void intializeSemaphoresAndQueues(void);

void getClockTimeTask(void);

void readBME280Task(void); 
void readSHT40Task(void);
void readSCD41Task(void);

void dataManagerTask(void);

void valuesChangedGUITask(void);
void timeChangedGUITask(void);

void joystickEvaluationTask(void);
void joystickActionGUITask(void);

void writeLogTask(void);

void writeValueToStorageTask(void);

#ifdef __cplusplus
}
#endif

#endif // TASKS_H
