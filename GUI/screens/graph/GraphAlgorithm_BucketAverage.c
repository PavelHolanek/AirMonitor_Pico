#include "GraphData.h"

// Two stage algorithm:
//
// 1. Bucketing. Every grid point owns a bucket one resolution step wide,
//    centred on it: [t_i - step/2, t_i + step/2). The buckets tile the axis
//    exactly, so every sample falls into precisely one of them. For each
//    non-empty bucket the arithmetic mean of both the values AND the times is
//    computed, giving one average point per bucket. Its time lies somewhere
//    inside the bucket, usually not on the grid point itself.
//
// 2. Interpolation. The grid values are linearly interpolated between the
//    average points. An average point can sit on either side of its own grid
//    point, so bucket i does not necessarily bracket grid point i: the segment
//    between two neighbouring average points may contain no grid point at all,
//    or several of them. That is why this stage searches the sequence of
//    average points rather than pairing them up with grid points by index.
//
// The average points are guaranteed to come out sorted by time: the average of
// bucket i cannot exceed t_i + step/2, which is exactly where bucket i+1
// starts. So a single forward walk is enough for stage 2.
//
// Edge handling:
//   - Bucket -1 (left of the graph) and bucket GRAPH_POINTS_COUNT (right of it)
//     are computed too. The first and last grid points sit in the middle of
//     their own buckets, so whenever their average point falls on the wrong
//     side, the neighbour just outside the graph is what makes them
//     interpolable at all.
//   - No extrapolation, same as the linear interpolation algorithm: a grid time
//     before the first or after the last average point stays invalid.
//   - Empty buckets are simply skipped, so the interpolation reaches over to
//     the next average point that exists and may fill several grid points from
//     one segment.

// Buckets -1 .. GRAPH_POINTS_COUNT inclusive.
#define BUCKET_COUNT (GRAPH_POINTS_COUNT + 2U)
#define BUCKET_FIRST (-1)

// Longest span between two average points that still gets interpolated, in
// resolution steps. 0 means no limit, which is what this algorithm wants:
// stage 1 already smooths the data and an outage is expected to show up as
// several empty buckets that the curve reaches over. Raise the linear
// interpolation algorithm's GRAPH_MAX_GAP_INTERVALS instead if you want the
// two to break the curve on the same gaps.
#define GRAPH_BUCKET_MAX_GAP_INTERVALS 0

typedef struct
{
    int32_t timeOffset;  // seconds relative to input->timeFrom, may be negative
    int32_t value;
} average_point_t;

// Floor division, so that negative offsets land in the bucket below zero
// instead of being truncated towards it.
static int32_t floorDiv(int32_t numerator, int32_t denominator)
{
    int32_t quotient = numerator / denominator;
    if (((numerator % denominator) != 0) && ((numerator < 0) != (denominator < 0)))
    {
        quotient--;
    }
    return quotient;
}

bool graph_computePoints_bucketAverage(const graph_input_t* input, graph_points_t* out)
{
    if (input == NULL || out == NULL || input->samples == NULL || input->sampleCount == 0U)
    {
        return false;
    }

    const int32_t step = (int32_t)graph_durationToSeconds(input->resolution);
    if (step <= 0)
    {
        return false;
    }
    const int32_t halfStep = step / 2;

    // ---------------------------------------------------------------------
    // Stage 1: average the samples into one point per bucket
    // ---------------------------------------------------------------------

    int64_t valueSum[BUCKET_COUNT] = {0};
    int64_t timeSum[BUCKET_COUNT] = {0};
    uint16_t count[BUCKET_COUNT] = {0};

    for (size_t i = 0U; i < input->sampleCount; ++i)
    {
        // Everything is done in seconds relative to timeFrom, which keeps the
        // averaging away from the year wrap in Time.
        const int32_t offset = graph_diffSeconds(input->samples[i].time, input->timeFrom);

        // Nearest grid point; the +halfStep shift makes the buckets tile
        // [b*step - halfStep, b*step + halfStep).
        const int32_t bucket = floorDiv(offset + halfStep, step);
        if (bucket < BUCKET_FIRST || bucket > (int32_t)GRAPH_POINTS_COUNT)
        {
            continue;
        }

        const size_t index = (size_t)(bucket - BUCKET_FIRST);
        valueSum[index] += (int64_t)input->samples[i].value;
        timeSum[index] += (int64_t)offset;
        count[index]++;
    }

    average_point_t averages[BUCKET_COUNT];
    size_t averageCount = 0U;

    for (size_t index = 0U; index < BUCKET_COUNT; ++index)
    {
        if (count[index] == 0U)
        {
            continue;
        }

        averages[averageCount].timeOffset = (int32_t)(timeSum[index] / (int64_t)count[index]);
        averages[averageCount].value = (int32_t)(valueSum[index] / (int64_t)count[index]);
        averageCount++;
    }

    if (averageCount == 0U)
    {
        return false;
    }

    // ---------------------------------------------------------------------
    // Stage 2: interpolate the grid values between the average points
    // ---------------------------------------------------------------------

    const int32_t maxGap = GRAPH_BUCKET_MAX_GAP_INTERVALS * step;
    bool anyValid = false;

    // Index of the first average point at or after the current grid time. Both
    // sequences are sorted, so the search never has to go back.
    size_t next = 0U;

    for (size_t i = 0U; i < GRAPH_POINTS_COUNT; ++i)
    {
        const int32_t target = (int32_t)i * step;

        while (next < averageCount && averages[next].timeOffset < target)
        {
            ++next;
        }

        if (next >= averageCount)
        {
            // No average point at or after this grid time, and the remaining
            // grid times are even later.
            break;
        }

        if (averages[next].timeOffset == target)
        {
            out->values[i] = averages[next].value;
            out->valid[i] = true;
            anyValid = true;
            continue;
        }

        if (next == 0U)
        {
            // Grid time lies left of the first average point. This is what
            // bucket -1 is for; if even that one was empty there is nothing to
            // interpolate from.
            continue;
        }

        const average_point_t* before = &averages[next - 1U];
        const average_point_t* after = &averages[next];

        const int32_t span = after->timeOffset - before->timeOffset;
        if (span <= 0)
        {
            // Two average points at the same instant; nothing to interpolate.
            out->values[i] = after->value;
            out->valid[i] = true;
            anyValid = true;
            continue;
        }
        if (maxGap > 0 && span > maxGap)
        {
            continue;
        }

        const int64_t rise = (int64_t)(after->value - before->value)
                             * (int64_t)(target - before->timeOffset);

        out->values[i] = before->value + (int32_t)(rise / (int64_t)span);
        out->valid[i] = true;
        anyValid = true;
    }

    return anyValid;
}
