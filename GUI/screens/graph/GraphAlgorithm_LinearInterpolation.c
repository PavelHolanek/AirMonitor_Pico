#include "GraphData.h"

// Value at every grid time linearly interpolated between the two neighbouring
// samples.
//
// Contract of every algorithm:
//   - out is already zeroed and all points marked invalid by graph_computePoints
//   - input->samples holds ALL samples, ascending by time, including the ones
//     outside [timeFrom, timeTo] - they are needed for the edge points
//   - input->timeTo may lie in the future (it is rounded up from currentTime),
//     so the trailing grid points may have no sample after them
//   - grid time of point i is graph_pointTime(input, i)
//   - compare times with graph_diffSeconds() / graph_isInFrame(), never with
//     compareTimes() - Time has no year and the frame may cross the new year
//   - return true when at least one point was marked valid
//
// Two deliberate refusals to invent data:
//   - no extrapolation. A grid time before the first sample or after the last
//     one stays invalid, so the curve simply ends where the measurements do.
//   - no interpolation across a long gap. When the two neighbouring samples are
//     further apart than GRAPH_MAX_GAP_INTERVALS resolution steps, the point
//     stays invalid and the curve breaks instead of drawing a straight line
//     through an outage. The threshold scales with the scope.
#define GRAPH_MAX_GAP_INTERVALS 2

bool graph_computePoints_linearInterpolation(const graph_input_t* input, graph_points_t* out)
{
    if (input == NULL || out == NULL || input->samples == NULL || input->sampleCount == 0U)
    {
        return false;
    }

    const int32_t maxGap = GRAPH_MAX_GAP_INTERVALS * (int32_t)graph_durationToSeconds(input->resolution);
    bool anyValid = false;

    // Index of the first sample at or after the current grid time. Grid times
    // only move forward, so the search resumes where the previous one stopped
    // instead of scanning the whole buffer for every point.
    size_t next = 0U;

    for (size_t i = 0U; i < GRAPH_POINTS_COUNT; ++i)
    {
        const Time gridTime = graph_pointTime(input, i);

        while (next < input->sampleCount
               && graph_diffSeconds(input->samples[next].time, gridTime) < 0)
        {
            ++next;
        }

        if (next >= input->sampleCount)
        {
            // Nothing measured at or after this grid time, and the remaining
            // grid times are even later.
            break;
        }

        const int32_t toNext = graph_diffSeconds(input->samples[next].time, gridTime);
        if (toNext == 0)
        {
            out->values[i] = input->samples[next].value;
            out->valid[i] = true;
            anyValid = true;
            continue;
        }

        if (next == 0U)
        {
            // Grid time lies before the first sample.
            continue;
        }

        const graph_sample_t* before = &input->samples[next - 1U];
        const graph_sample_t* after = &input->samples[next];

        const int32_t span = graph_diffSeconds(after->time, before->time);
        if (span <= 0 || span > maxGap)
        {
            // Samples out of order, or a gap too long to interpolate across.
            continue;
        }

        const int32_t offset = graph_diffSeconds(gridTime, before->time);
        const int64_t rise = (int64_t)(after->value - before->value) * (int64_t)offset;

        out->values[i] = before->value + (int32_t)(rise / (int64_t)span);
        out->valid[i] = true;
        anyValid = true;
    }

    return anyValid;
}
