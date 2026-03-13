#include "srv_threshold.h"
#include <string.h>

void srvThresholdInit(threshold_channel_t *ch,
                      float high_thresh,
                      float low_thresh,
                      int   debounce_samples) {
    ch->high_thresh      = high_thresh;
    ch->low_thresh       = low_thresh;
    ch->debounce_samples = debounce_samples;
    ch->alert            = false;
    ch->debounce_counter = 0;
}

bool srvThresholdUpdate(threshold_channel_t *ch, float value) {
    if (!ch->alert) {
        // Looking for sustained rise above high_thresh
        if (value > ch->high_thresh) {
            ch->debounce_counter++;
            if (ch->debounce_counter >= ch->debounce_samples) {
                ch->alert            = true;
                ch->debounce_counter = 0;
            }
        } else {
            ch->debounce_counter = 0;
        }
    } else {
        // Looking for sustained drop below low_thresh
        if (value < ch->low_thresh) {
            ch->debounce_counter++;
            if (ch->debounce_counter >= ch->debounce_samples) {
                ch->alert            = false;
                ch->debounce_counter = 0;
            }
        } else {
            ch->debounce_counter = 0;
        }
    }
    return ch->alert;
}

bool srvThresholdIsAlert(const threshold_channel_t *ch) {
    return ch->alert;
}

int srvThresholdDebounceCount(const threshold_channel_t *ch) {
    return ch->debounce_counter;
}
