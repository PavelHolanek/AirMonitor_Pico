#ifndef JOYSTICK_H
#define JOYSTICK_H

// Two potentiometers on the ADC give the direction, the comparator circuit on
// GPIO_MOVE_PIN and the button on GPIO_PUSH_PIN raise an interrupt. The driver
// reports every change to one listener and keeps a snapshot anyone can read.
// It knows nothing about the GUI, and this header pulls in no FreeRTOS, so the
// GUI can include it.

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum
{
    JOYSTICK_NONE = 0,  // in the centre
    JOYSTICK_RIGHT,
    JOYSTICK_UP,
    JOYSTICK_LEFT,
    JOYSTICK_DOWN
} JOYSTICK_DIRECTION;

typedef enum
{
    JOYSTICK_EVENT_DIRECTION = 0,  // direction changed, JOYSTICK_NONE = back in the centre
    JOYSTICK_EVENT_BUTTON_DOWN,
    JOYSTICK_EVENT_BUTTON_UP
} JOYSTICK_EVENT_TYPE;

typedef struct
{
    JOYSTICK_EVENT_TYPE type;
    JOYSTICK_DIRECTION direction;  // the direction after the event
    int16_t horizontal;            // calibrated axes at that moment
    int16_t vertical;
    uint32_t timeMs;               // since boot
} JoystickEvent;

// What the listener has been told so far - the filtered state, not the raw pins.
typedef struct
{
    JOYSTICK_DIRECTION direction;
    uint32_t directionSinceMs;
    bool pressed;
    uint32_t pressedSinceMs;
    int16_t horizontal;            // latest sample
    int16_t vertical;
} JoystickSnapshot;

// Runs on the joystick task. Must not block - hand the event over and return.
typedef void (*JoystickListener)(const JoystickEvent* event);

// ADC calibration and the GPIO interrupts. Call before the scheduler starts,
// with the stick at rest.
void joystick_init(JoystickListener listener);

// Sleeps while nobody touches the joystick and samples it while they do.
void joystickTask(void*);

// Safe from any task.
void joystick_getState(JoystickSnapshot* out);

#ifdef __cplusplus
}
#endif

#endif // JOYSTICK_H
