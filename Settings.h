#ifndef SETTINGS_H
#define SETTINGS_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

extern uint32_t sensorsMeassurementPeriod;
extern uint32_t timeUpdatePeriod;
extern uint32_t idleTime;

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

extern GRAPH_ALGORITHM graphAlgorithm;

#ifdef __cplusplus
}
#endif

#endif // SETTINGS_H
