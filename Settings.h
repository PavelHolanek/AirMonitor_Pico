#ifndef SETTINGS_H
#define SETTINGS_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

// Runtime-tunable settings in milliseconds. Definitions live in Settings.c,
// initial values are overwritten in tasks_init(). uint32_t because a uint16_t
// tops out at 65.5 s and the measurement period is longer than that.
extern uint32_t sensorsMeassurementPeriod;
extern uint32_t timeUpdatePeriod;
extern uint32_t idleTime;

// Selects which algorithm turns the raw (time, value) samples into the fixed
// grid of graph points. All algorithms take the same input (graph_input_t) and
// fill the same output (graph_points_t), see GraphData.h.
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
