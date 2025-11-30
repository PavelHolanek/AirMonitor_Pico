#ifndef TASKS_H
#define TASKS_H

#ifdef __cplusplus
extern "C" {
#endif

#include "Clock.h"
 
#include "FreeRTOS.h"
#include "queue.h"
#include "semphr.h"

#define TICKS_TO_WAIT 1000

extern SemaphoreHandle_t i2c0_semaphore;
extern SemaphoreHandle_t i2c1_semaphore;
extern SemaphoreHandle_t spi0_semaphore;
extern SemaphoreHandle_t spi1_semaphore;

extern QueueHandle_t BME280DataQueue;
extern QueueHandle_t SHT40DataQueue;
extern QueueHandle_t SDC41DataQueue;

extern QueueHandle_t TemperatureQueue;
extern QueueHandle_t PreassureQueue;
extern QueueHandle_t HumidityQueue;
extern QueueHandle_t CO2Queue;

extern SemaphoreHandle_t TimeRequestSemaphore;
extern QueueHandle_t CurrentTimeQueue;

extern QueueHandle_t LogsToStoreQueue;

extern QueueHandle_t JoystickStateQueue;

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
