#ifndef TASKS_H
#define TASKS_H

#ifdef __cplusplus
extern "C" {
#endif

#include "Clock.h"
 
#include "FreeRTOS.h"
#include "queue.h"
#include "semphr.h"
#include "event_groups.h"

#define TICKS_TO_WAIT 1000
#define JOYSTICK_DEBOUNCING_PERIOD pdMS_TO_TICKS(500)

extern SemaphoreHandle_t i2c0_mutex;
extern SemaphoreHandle_t i2c1_mutex;
extern SemaphoreHandle_t spi0_mutex;
extern SemaphoreHandle_t spi1_mutex;

extern QueueHandle_t bmp280DataQueue;
extern QueueHandle_t SHT40DataQueue;
extern QueueHandle_t SDC41DataQueue;

extern QueueHandle_t TemperatureQueue;
extern QueueHandle_t PreassureQueue;
extern QueueHandle_t HumidityQueue;
extern QueueHandle_t CO2Queue;

extern SemaphoreHandle_t TimeRequestSemaphore;
extern QueueHandle_t CurrentTimeQueue;
extern SemaphoreHandle_t TimeSetRequestSemaphore;
extern QueueHandle_t TimeToSetQueue;

extern QueueHandle_t LogsToStoreQueue;

extern SemaphoreHandle_t JoystickMoveInteruptionSemaphore;
extern SemaphoreHandle_t JoystickPressInteruptionSemaphore;
extern QueueHandle_t JoystickStateQueue;

extern EventGroupHandle_t JoystickEventGroup;

void intializeSemaphoresAndQueues();

void getClockTimeTask(void*);
void setClockTimeTask(void*);

void readbmp280Task(void*); 
void readSHT40Task(void*);
void readSCD41Task(void*);

void dataManagerTask(void*);

void valuesChangedGUITask(void*);
void timeChangedGUITask(void*);

void joystickPressedTask(void*);
void joystickMovedTask(void*);
void joystickEvaluationTask(void*);

void writeLogTask(void*);

void writeValueToStorageTask(void*);

#ifdef __cplusplus
}
#endif

#endif // TASKS_H
