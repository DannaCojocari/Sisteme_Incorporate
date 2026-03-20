#ifndef SRV_SIGNAL_CONDITIONING_H
#define SRV_SIGNAL_CONDITIONING_H

#include <Arduino.h>

#define MEDIAN_FILTER_SIZE 5

typedef struct {
    float buffer[MEDIAN_FILTER_SIZE];
    int   index;
    int   count;
} median_filter_t;

// Intermediate values returned by the conditioning pipeline
typedef struct {
    float saturated;   // after saturation
    float median;      // after median filter
    float wma;         // after weighted moving average
    float final;       // final result (combination of median + wma)
} conditioning_result_t;

void               srvSignalConditioningInit(median_filter_t *filter);
conditioning_result_t srvSignalConditioningApply(median_filter_t *filter, float newValue);

#endif // SRV_SIGNAL_CONDITIONING_H