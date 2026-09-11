#ifndef SETTINGS_H
#define SETTINGS_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

extern uint32_t sensorsMeassurementPeriod;
extern uint32_t timeUpdatePeriod;
extern uint32_t idleTime;

extern  uint16_t joystickCalibration0;
extern  uint16_t joystickCalibration1;
typedef enum
{
    // Value at every grid time linearly interpolated between the two
    // neighbouring raw samples.
    GRAPH_ALGORITHM_LINEAR_INTERPOLATION = 0,

    // Raw samples are first averaged into one point per resolution wide bucket,
    // then the grid values are interpolated between those average points.
    GRAPH_ALGORITHM_BUCKET_AVERAGE,

    GRAPH_ALGORITHM_COUNT
} GRAPH_ALGORITHM;

typedef enum {
    INTERVAL_2_MIN = 0, 
    INTERVAL_5_MIN, 
    INTERVAL_10_MIN 
} INTERVAL;

extern GRAPH_ALGORITHM graphAlgorithm;
extern INTERVAL meassurementInterval;

extern bool useFixedValuesSpanForGraph;
extern bool developerMode;
extern bool screenBacklightWhenIdle;

extern uint32_t temperatureCalibration;
extern uint32_t preassureCalibration;
extern uint32_t humidityCalibration;
extern uint32_t co2Calibration;

#ifdef __cplusplus
}
#endif

#endif // SETTINGS_H
