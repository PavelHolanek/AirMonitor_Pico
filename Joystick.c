#include "Joystick.h"
#include "Pinout.h"
#include "Settings.h"
#include "FreeRTOS.h"
#include "task.h"
#include "hardware/adc.h"
#include "hardware/gpio.h"
#include "pico/time.h"
#include <stdlib.h>

#define JOYSTICK_CALIBRATION_SAMPLES 16U
#define JOYSTICK_CALIBRATION_MIN 1500U
#define JOYSTICK_CALIBRATION_MAX 2600U

// Sampling period while the joystick is touched
#define JOYSTICK_POLL_MS 20U

// A change comes at the earliest this long after the previous one, the last
// line of defence against bounce the hardware lets through.
#define JOYSTICK_LOCKOUT_MS 200U

// Back to rest has to be seen this many samples in a row
#define JOYSTICK_RELEASE_SAMPLES 3U

// ADC counts from the calibrated centre. Leaving the centre takes more than
// coming back to it, so a reading hovering around one threshold does not
// flicker. Both have to stay under the deflection at which the comparators trip.
#define JOYSTICK_DEAD_ZONE 200
#define JOYSTICK_DEAD_ZONE_RELEASE 150

// Near a diagonal the axis in charge keeps it until the other one leads by this much
#define JOYSTICK_AXIS_HYSTERESIS 100

// Debounces one input: the stick direction or the button.
typedef struct
{
    uint8_t reported;
    uint8_t restSamples;
    uint32_t changedMs;
} InputFilter;

static JoystickListener listener = NULL;
static TaskHandle_t volatile joystickTaskHandle = NULL;

static InputFilter stickFilter = { JOYSTICK_NONE, 0U, 0U };
static InputFilter buttonFilter = { 0U, 0U, 0U };
static JOYSTICK_DIRECTION measuredDirection = JOYSTICK_NONE;
static bool measuredPressed = false;

// Written by the joystick task, read by anyone, always inside a critical section
static JoystickSnapshot snapshot = { JOYSTICK_NONE, 0U, false, 0U, 0, 0 };

static uint32_t nowMs(void)
{
    return to_ms_since_boot(get_absolute_time());
}

// Idle offset of one joystick axis, averaged over several readings
static uint16_t calibrateAxis(uint8_t adcInput, uint16_t originalValue)
{
    uint32_t sum = 0U;
    uint16_t accepted = 0U;

    adc_select_input(adcInput);

    for (uint16_t i = 0U; i < JOYSTICK_CALIBRATION_SAMPLES; i++)
    {
        const uint16_t value = adc_read();
        if (value >= JOYSTICK_CALIBRATION_MIN && value <= JOYSTICK_CALIBRATION_MAX)
        {
            sum += value;
            accepted++;
        }
    }
    if (accepted == 0U)
    {
        return originalValue;
    }

    return (uint16_t)(sum / accepted);
}

// Only wakes the task - it reads the pins and the ADC itself.
static void joystickIrqCallback(uint gpio, uint32_t events)
{
    (void)events;
    TaskHandle_t task = joystickTaskHandle;
    if ((gpio != GPIO_PUSH_PIN && gpio != GPIO_MOVE_PIN) || task == NULL)
    {
        return;
    }

    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    vTaskNotifyGiveFromISR(task, &xHigherPriorityTaskWoken);
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

// Screen directions from the calibrated axes. The stick is mounted turned, so
// the vertical ADC axis is right/left and the horizontal one up/down.
static JOYSTICK_DIRECTION classify(int16_t horizontal, int16_t vertical, JOYSTICK_DIRECTION current)
{
    const int32_t absHorizontal = abs(horizontal);
    const int32_t absVertical = abs(vertical);
    const int32_t magnitude = (absHorizontal > absVertical) ? absHorizontal : absVertical;

    const int32_t deadZone = (current == JOYSTICK_NONE) ? JOYSTICK_DEAD_ZONE : JOYSTICK_DEAD_ZONE_RELEASE;
    if (magnitude < deadZone)
    {
        return JOYSTICK_NONE;
    }

    bool rightLeft;
    if (current == JOYSTICK_RIGHT || current == JOYSTICK_LEFT)
    {
        rightLeft = (absVertical + JOYSTICK_AXIS_HYSTERESIS >= absHorizontal);
    }
    else if (current == JOYSTICK_UP || current == JOYSTICK_DOWN)
    {
        rightLeft = (absVertical > absHorizontal + JOYSTICK_AXIS_HYSTERESIS);
    }
    else
    {
        rightLeft = (absVertical > absHorizontal);
    }

    if (rightLeft)
    {
        return (vertical > 0) ? JOYSTICK_RIGHT : JOYSTICK_LEFT;
    }
    return (horizontal > 0) ? JOYSTICK_UP : JOYSTICK_DOWN;
}

// True when the reported state moves to measured. Back to rest has to hold for
// JOYSTICK_RELEASE_SAMPLES, so a glitch while held does not end the hold and
// start a new one. A change held back by the lockout is not lost: whatever the
// input shows once the lockout is over gets reported then.
static bool filterInput(InputFilter* filter, uint8_t measured, uint8_t rest, uint32_t now)
{
    if (measured == filter->reported)
    {
        filter->restSamples = 0U;
        return false;
    }

    if (measured == rest)
    {
        if (filter->restSamples < JOYSTICK_RELEASE_SAMPLES)
        {
            filter->restSamples++;
        }
        if (filter->restSamples < JOYSTICK_RELEASE_SAMPLES)
        {
            return false;
        }
    }
    else
    {
        filter->restSamples = 0U;
    }

    if ((uint32_t)(now - filter->changedMs) < JOYSTICK_LOCKOUT_MS)
    {
        return false;
    }

    filter->reported = measured;
    filter->changedMs = now;
    filter->restSamples = 0U;
    return true;
}

static void emit(JOYSTICK_EVENT_TYPE type, int16_t horizontal, int16_t vertical, uint32_t now)
{
    if (!listener)
    {
        return;
    }
    const JoystickEvent event = { type, (JOYSTICK_DIRECTION)stickFilter.reported, horizontal, vertical, now };
    listener(&event);
}

static void sample(void)
{
    const uint32_t now = nowMs();

    adc_select_input(0);
    const int16_t horizontal = (int16_t)(adc_read() - joystickCalibration0);
    adc_select_input(1);
    const int16_t vertical = (int16_t)(adc_read() - joystickCalibration1);

    measuredDirection = classify(horizontal, vertical, measuredDirection);
    measuredPressed = (gpio_get(GPIO_PUSH_PIN) == 0);  // idles high

    const bool buttonChanged = filterInput(&buttonFilter, measuredPressed ? 1U : 0U, 0U, now);
    const bool directionChanged = filterInput(&stickFilter, (uint8_t)measuredDirection, JOYSTICK_NONE, now);

    taskENTER_CRITICAL();
    snapshot.direction = (JOYSTICK_DIRECTION)stickFilter.reported;
    snapshot.directionSinceMs = stickFilter.changedMs;
    snapshot.pressed = (buttonFilter.reported != 0U);
    snapshot.pressedSinceMs = buttonFilter.changedMs;
    snapshot.horizontal = horizontal;
    snapshot.vertical = vertical;
    taskEXIT_CRITICAL();

    // The button first: a press that tilts the stick is a press.
    if (buttonChanged)
    {
        emit(buttonFilter.reported ? JOYSTICK_EVENT_BUTTON_DOWN : JOYSTICK_EVENT_BUTTON_UP,
             horizontal, vertical, now);
    }
    if (directionChanged)
    {
        emit(JOYSTICK_EVENT_DIRECTION, horizontal, vertical, now);
    }
}

// The ADC decides the direction, but a pin still held low means something is
// going on even when the reading says centre.
static bool atRest(void)
{
    return measuredDirection == JOYSTICK_NONE && stickFilter.reported == JOYSTICK_NONE
        && !measuredPressed && buttonFilter.reported == 0U
        && gpio_get(GPIO_MOVE_PIN) && gpio_get(GPIO_PUSH_PIN);
}

void joystick_init(JoystickListener eventListener)
{
    listener = eventListener;

    adc_init();
    adc_gpio_init(ADC0_PIN);
    adc_gpio_init(ADC1_PIN);

    joystickCalibration0 = calibrateAxis(0, joystickCalibration0);
    joystickCalibration1 = calibrateAxis(1, joystickCalibration1);

    gpio_init(GPIO_PUSH_PIN);
    gpio_init(GPIO_MOVE_PIN);
    gpio_set_irq_enabled_with_callback(GPIO_PUSH_PIN, GPIO_IRQ_EDGE_FALL, true, &joystickIrqCallback);
    gpio_set_irq_enabled_with_callback(GPIO_MOVE_PIN, GPIO_IRQ_EDGE_FALL, true, &joystickIrqCallback);
}

void joystickTask(void*)
{
    joystickTaskHandle = xTaskGetCurrentTaskHandle();

    for (;;)
    {
        sample();
        if (atRest())
        {
            // An edge between atRest() and here is not lost, the notification waits.
            ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
        }
        else
        {
            vTaskDelay(pdMS_TO_TICKS(JOYSTICK_POLL_MS));
        }
    }
}

void joystick_getState(JoystickSnapshot* out)
{
    if (!out)
    {
        return;
    }
    taskENTER_CRITICAL();
    *out = snapshot;
    taskEXIT_CRITICAL();
}
