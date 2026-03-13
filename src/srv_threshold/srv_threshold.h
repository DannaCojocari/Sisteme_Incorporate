#ifndef SRV_THRESHOLD_H
#define SRV_THRESHOLD_H

#include <stdbool.h>

/**
 * Generic hysteresis + debounce signal conditioning service.
 *
 * Each channel is an independent instance that implements the state machine:
 *
 *   State: NOT ALERTING
 *     value > high_thresh → debounce_counter++
 *     value ≤ high_thresh → debounce_counter = 0  (not yet stable above)
 *     debounce_counter ≥ debounce_samples → alert = true, counter = 0
 *
 *   State: ALERTING
 *     value < low_thresh  → debounce_counter++
 *     value ≥ low_thresh  → debounce_counter = 0  (hysteresis guard)
 *     debounce_counter ≥ debounce_samples → alert = false, counter = 0
 *
 * The asymmetric high/low thresholds implement hysteresis, preventing rapid
 * oscillation when the signal hovers near a single boundary.
 */
typedef struct {
    float high_thresh;       // alert activates  when value > high_thresh
    float low_thresh;        // alert deactivates when value < low_thresh
    int   debounce_samples;  // consecutive confirmations required

    // ── Internal state (managed by srvThresholdUpdate) ────────────────────
    bool alert;
    int  debounce_counter;
} threshold_channel_t;

/**
 * Initialise a threshold channel.
 * Must be called before the first srvThresholdUpdate().
 */
void srvThresholdInit(threshold_channel_t *ch,
                      float high_thresh,
                      float low_thresh,
                      int   debounce_samples);

/**
 * Feed a new sample into the channel.
 * Applies the hysteresis + debounce state machine and updates ch->alert.
 * Returns the current alert state (true = threshold exceeded).
 */
bool srvThresholdUpdate(threshold_channel_t *ch, float value);

/**
 * Returns the current alert state without advancing the state machine.
 */
bool srvThresholdIsAlert(const threshold_channel_t *ch);

/**
 * Returns the current debounce counter (useful for displaying progress).
 */
int  srvThresholdDebounceCount(const threshold_channel_t *ch);

#endif // SRV_THRESHOLD_H
