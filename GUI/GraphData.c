#include "GraphData.h"

// Durations encoded as an offset from {1, 1, 0, 0, 0}, so day = 2 means one day.
//
// TIME_RESOLUTION drives the tables: every scope spans exactly
// GRAPH_INTERVALS_COUNT of its own resolution steps. Each resolution divides
// 24 h, so grid lines always land on whole clock times.
const Time TIMES_SCOPES[GRAPH_SCOPES_COUNT] =
{
    {1, 1,  1, 40, 0},  //  1 h 40 min  (20 x  5 min)
    {1, 1,  3, 20, 0},  //  3 h 20 min  (20 x 10 min)
    {1, 1,  6, 40, 0},  //  6 h 40 min  (20 x 20 min)
    {1, 1, 20,  0, 0},  // 20 h         (20 x  1 h)
    {1, 2, 16,  0, 0},  //  1 d 16 h    (20 x  2 h)
    {1, 4,  8,  0, 0},  //  3 d  8 h    (20 x  4 h)
};

const Time TIME_RESOLUTION[GRAPH_SCOPES_COUNT] =
{
    {1, 1, 0,  5, 0},   //  5 min
    {1, 1, 0, 10, 0},   // 10 min
    {1, 1, 0, 20, 0},   // 20 min
    {1, 1, 1,  0, 0},   //  1 h
    {1, 1, 2,  0, 0},   //  2 h
    {1, 1, 4,  0, 0},   //  4 h
};

uint32_t graph_durationToSeconds(Time duration)
{
    const uint32_t days = (duration.day > 0U) ? (uint32_t)(duration.day - 1U) : 0U;
    return (((days * 24U) + (uint32_t)duration.hour) * 60U + (uint32_t)duration.minute) * 60U
           + (uint32_t)duration.second;
}

// Clock.c keeps a nominal 365 day year and wraps addTime() around it, so two
// times a few hours apart can differ by almost a whole year after subtraction.
#define GRAPH_SECONDS_IN_YEAR (365 * 24 * 60 * 60)

int32_t graph_diffSeconds(Time later, Time earlier)
{
    int32_t diff = (int32_t)timeToTotalSeconds(later) - (int32_t)timeToTotalSeconds(earlier);

    if (diff > (GRAPH_SECONDS_IN_YEAR / 2))
    {
        diff -= GRAPH_SECONDS_IN_YEAR;
    }
    else if (diff < -(GRAPH_SECONDS_IN_YEAR / 2))
    {
        diff += GRAPH_SECONDS_IN_YEAR;
    }

    return diff;
}

bool graph_isInFrame(const graph_input_t* input, Time time)
{
    if (input == NULL)
    {
        return false;
    }

    const int32_t offset = graph_diffSeconds(time, input->timeFrom);
    return (offset >= 0) && (offset <= (int32_t)graph_durationToSeconds(input->span));
}

Time graph_roundUpToResolution(Time time, Time resolution)
{
    const uint32_t step = graph_durationToSeconds(resolution);
    if (step == 0U)
    {
        return time;
    }

    const uint32_t secondsOfDay =
        (((uint32_t)time.hour * 60U) + (uint32_t)time.minute) * 60U + (uint32_t)time.second;
    const uint32_t rounded = ((secondsOfDay + step - 1U) / step) * step;

    Time midnight = time;
    midnight.hour = 0U;
    midnight.minute = 0U;
    midnight.second = 0U;

    // addSeconds carries over into the next day when rounded reaches 24:00.
    return addSeconds(midnight, (int32_t)rounded);
}

Time graph_pointTime(const graph_input_t* input, size_t index)
{
    if (input == NULL)
    {
        Time zero = {1U, 1U, 0U, 0U, 0U};
        return zero;
    }

    const uint32_t step = graph_durationToSeconds(input->resolution);
    return addSeconds(input->timeFrom, (int32_t)(step * (uint32_t)index));
}

bool graph_makeInputEndingAt(
    uint8_t scope,
    Time currentTime,
    Time timeTo,
    const graph_sample_t* samples,
    size_t sampleCount,
    graph_input_t* out)
{
    if (out == NULL || scope >= GRAPH_SCOPES_COUNT)
    {
        return false;
    }

    out->span = TIMES_SCOPES[scope];
    out->resolution = TIME_RESOLUTION[scope];
    out->currentTime = currentTime;
    out->timeTo = timeTo;
    out->timeFrom = addSeconds(timeTo, -(int32_t)graph_durationToSeconds(out->span));
    out->samples = samples;
    out->sampleCount = (samples != NULL) ? sampleCount : 0U;

    return true;
}

bool graph_makeRecentInput(
    uint8_t scope,
    Time currentTime,
    const graph_sample_t* samples,
    size_t sampleCount,
    graph_input_t* out)
{
    if (scope >= GRAPH_SCOPES_COUNT)
    {
        return false;
    }

    const Time timeTo = graph_roundUpToResolution(currentTime, TIME_RESOLUTION[scope]);
    return graph_makeInputEndingAt(scope, currentTime, timeTo, samples, sampleCount, out);
}

bool graph_computePoints(const graph_input_t* input, GRAPH_ALGORITHM algorithm, graph_points_t* out)
{
    if (out == NULL)
    {
        return false;
    }

    for (size_t i = 0U; i < GRAPH_POINTS_COUNT; ++i)
    {
        out->values[i] = 0;
        out->valid[i] = false;
    }

    if (input == NULL)
    {
        return false;
    }

    switch (algorithm)
    {
        case GRAPH_ALGORITHM_LINEAR_INTERPOLATION:
            return graph_computePoints_linearInterpolation(input, out);

        case GRAPH_ALGORITHM_BUCKET_AVERAGE:
            return graph_computePoints_bucketAverage(input, out);

        default:
            return false;
    }
}
