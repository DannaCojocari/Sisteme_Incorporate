#include "srv_signal_conditioning.h"
#include <stdlib.h>
#include <string.h>

static int compare_float(const void *a, const void *b) {
    float fa = *(const float *)a;
    float fb = *(const float *)b;
    return (fa > fb) - (fa < fb);
}

void srvSignalConditioningInit(median_filter_t *filter) {
    filter->index = 0;
    filter->count = 0;
}

conditioning_result_t srvSignalConditioningApply(median_filter_t *filter, float newValue) {
    conditioning_result_t result;

    // 1. Saturation
    if (newValue > 100.0f) newValue = 100.0f;
    if (newValue < 0.0f) newValue = 0.0f;
    result.saturated = newValue;

    // 2. Add to circular buffer
    filter->buffer[filter->index] = newValue;
    filter->index = (filter->index + 1) % MEDIAN_FILTER_SIZE;
    if (filter->count < MEDIAN_FILTER_SIZE) {
        filter->count++;
    }

    // 3. Median filter
    float sorted[MEDIAN_FILTER_SIZE];
    memcpy(sorted, filter->buffer, (size_t)filter->count * sizeof(float));
    qsort(sorted, (size_t)filter->count, sizeof(float), compare_float);

    float median;
    if (filter->count % 2 == 1) {
        median = sorted[filter->count / 2];
    } else {
        median = (sorted[filter->count / 2 - 1] + sorted[filter->count / 2]) * 0.5f;
    }
    result.median = median;

    // 4. Weighted Moving Average on chronological buffer
    float wma = 0.0f;
    float weight_sum = 0.0f;
    int oldest = (filter->count < MEDIAN_FILTER_SIZE) ? 0 : filter->index;

    for (int i = 0; i < filter->count; i++) {
        int   slot = (oldest + i) % MEDIAN_FILTER_SIZE;
        float w    = (float)(i + 1);
        wma        += filter->buffer[slot] * w;
        weight_sum += w;
    }
    wma /= weight_sum;
    result.wma = wma;

    // 5. Final result: median + WMA combined
    result.final = median * 0.6f + wma * 0.4f;

    return result;
}