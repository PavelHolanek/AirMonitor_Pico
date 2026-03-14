#ifndef GRAPH_DATA_H
#define GRAPH_DATA_H

#include <stddef.h>
#include <stdint.h>
#include "Clock.h"

#ifdef __cplusplus
extern "C" {
#endif

#define GRAPH_WIDGET_MAX_POINTS 22U

typedef struct
{
    Time time;
    int32_t temperature_c;
    int32_t humidity_rh;
    int32_t pressure_pa;
    int32_t co2_ppm;
} gui_graph_sample_t;

#ifdef __cplusplus
}
#endif

#endif // GRAPH_DATA_H
