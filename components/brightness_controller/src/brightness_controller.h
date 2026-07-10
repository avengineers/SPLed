#ifndef BRIGHTNESS_CONTROLLER_H
#define BRIGHTNESS_CONTROLLER_H

#include "rte.h"

#ifdef CONFIG_BRIGHTNESS_ADJUSTMENT_AUTOMATIC

typedef struct
{
    uint32_t ticksCounter;
    const uint32_t period;
    const uint32_t halfPeriod;
    const brightness_t maxBrightness;
    const brightness_t minBrightness;
} BrightnessAdjustmentData;

#endif /* CONFIG_BRIGHTNESS_ADJUSTMENT_AUTOMATIC */

void brightnessController(void);

#endif /* BRIGHTNESS_CONTROLLER_H */
