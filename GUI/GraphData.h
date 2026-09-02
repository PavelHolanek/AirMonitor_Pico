#ifndef GRAPH_DATA_H
#define GRAPH_DATA_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "Clock.h"
#include "Base.h"
#include "Settings.h"

#ifdef __cplusplus
extern "C" {
#endif

// -----------------------------------------------------------------------------
// Time frame of the graph
// -----------------------------------------------------------------------------

// Number of selectable time scopes (30 min ... 1 day).
#define GRAPH_SCOPES_COUNT 6U

// Invariant: every scope divides its span into the same number of resolution
// steps, so the number of computed points never depends on the scope. Checked
// by _Static_assert in GraphData.c.
// 20 intervals = 21 points, which is exactly 21 px per interval on the current
// 420 px wide plot.
#define GRAPH_INTERVALS_COUNT 20U
#define GRAPH_POINTS_COUNT (GRAPH_INTERVALS_COUNT + 1U)

// Both tables hold durations, not absolute times. A duration is encoded as an
// offset from {month = 1, day = 1, 0, 0, 0}, i.e. day = 2 means "one day".
//   TIMES_SCOPES[s]    - total length of the horizontal axis
//   TIME_RESOLUTION[s] - distance between two neighbouring graph points
extern const Time TIMES_SCOPES[GRAPH_SCOPES_COUNT];
extern const Time TIME_RESOLUTION[GRAPH_SCOPES_COUNT];

// -----------------------------------------------------------------------------
// Input / output of the coordinate computation
// -----------------------------------------------------------------------------

// One measured value at one point in time.
typedef struct
{
    Time time;
    int32_t value;
} graph_sample_t;

// Everything an algorithm is allowed to look at. Identical for all algorithms.
typedef struct
{
    Time span;        // TIMES_SCOPES[scope]
    Time resolution;  // TIME_RESOLUTION[scope]
    Time currentTime; // "now"; timeTo may lie ahead of it
    Time timeFrom;    // left edge of the horizontal axis
    Time timeTo;      // right edge of the horizontal axis (timeFrom + span)

    // All available samples, ascending by time. Samples outside
    // [timeFrom, timeTo] are included on purpose: the algorithm needs the
    // neighbours just outside the frame to produce the edge points.
    const graph_sample_t* samples;
    size_t sampleCount;
} graph_input_t;

// Result: one value per grid time, i-th value belongs to graph_pointTime(in, i).
typedef struct
{
    int32_t values[GRAPH_POINTS_COUNT];
    bool valid[GRAPH_POINTS_COUNT];
} graph_points_t;

// -----------------------------------------------------------------------------
// Time frame helpers
// -----------------------------------------------------------------------------

// Length of a duration-encoded Time in seconds (month is ignored).
uint32_t graph_durationToSeconds(Time duration);

// Signed distance later - earlier in seconds, correct across the new year.
// Time carries no year, so a raw subtraction jumps by a whole year when the
// frame crosses 31 Dec / 1 Jan; a difference larger than half a year is treated
// as such a wrap. Always use this instead of compareTimes()/diffSeconds() when
// working with graph times.
int32_t graph_diffSeconds(Time later, Time earlier);

// True when time lies inside [timeFrom, timeTo], new year aware.
bool graph_isInFrame(const graph_input_t* input, Time time);

// Rounds up (towards the future) to the next multiple of resolution, anchored
// at midnight. 02:43 becomes 02:45 / 02:50 / 03:00 / 03:00 / 04:00 / 04:00 for
// scope 0...5.
Time graph_roundUpToResolution(Time time, Time resolution);

// Absolute time of the index-th grid point, index in [0, GRAPH_POINTS_COUNT).
Time graph_pointTime(const graph_input_t* input, size_t index);

// Fills input for the "show the most recent data" mode: timeTo is derived from
// currentTime by rounding up to TIME_RESOLUTION[scope].
bool graph_makeRecentInput(
    uint8_t scope,
    Time currentTime,
    const graph_sample_t* samples,
    size_t sampleCount,
    graph_input_t* out);

// Fills input for the "scrolled into the past" mode with an explicit right edge.
bool graph_makeInputEndingAt(
    uint8_t scope,
    Time currentTime,
    Time timeTo,
    const graph_sample_t* samples,
    size_t sampleCount,
    graph_input_t* out);

// -----------------------------------------------------------------------------
// Algorithms
// -----------------------------------------------------------------------------

// Clears out and dispatches to the implementation selected by algorithm.
// Returns false when the input is unusable or the algorithm produced nothing.
bool graph_computePoints(const graph_input_t* input, GRAPH_ALGORITHM algorithm, graph_points_t* out);

// Concrete implementations. Each one gets a zeroed out and may leave any point
// invalid. One file per algorithm.
bool graph_computePoints_linearInterpolation(const graph_input_t* input, graph_points_t* out);

#ifdef __cplusplus
}
#endif

#endif // GRAPH_DATA_H
