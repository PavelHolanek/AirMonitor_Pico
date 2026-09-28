#ifndef TASKS_H
#define TASKS_H

#ifdef __cplusplus
extern "C" {
#endif

#include "Clock.h"
#include "Joystick.h"
 
#include "FreeRTOS.h"
#include "queue.h"
#include "semphr.h"

#define TICKS_TO_WAIT 1000
#define GUI_EVENT_QUEUE_LENGTH 16

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

extern SemaphoreHandle_t TimeSetRequestSemaphore;
extern QueueHandle_t TimeToSetQueue;

extern QueueHandle_t LogsToStoreQueue;

extern QueueHandle_t GuiEventQueue;

void intializeSemaphoresAndQueues();

void setClockTimeTask(void*);

void readbmp280Task(void*); 
void readSHT40Task(void*);
void readSCD41Task(void*);

void dataManagerTask(void*);

void timeChangedGUITask(void*);

// The joystick driver's listener: hands the event over to guiTask.
void postJoystickEventToGui(const JoystickEvent* event);

// The only task that handles joystick events and GUI timers.
void guiTask(void*);

void writeLogTask(void*);

void writeValueToStorageTask(void*);

#ifdef __cplusplus
}
#endif

#endif // TASKS_H
