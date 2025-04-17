#ifndef __brightness_controller_h__
#define __brightness_controller_h__

#include "autoconf.h"

#ifdef CONFIG_BRIGHTNESS_ADJUSTMENT_PERIOD

typedef struct {
    unsigned int timeCounter;
    const unsigned int period;
    const unsigned int halfPeriod;
    const unsigned int maxBrightness;
    const unsigned int minBrightness;
} BrightnessAdjustmentData;

#endif

void brightnessController(void);

#endif // __brightness_controller_h__
