/**
 * @file brightness_controller.c
 * @brief Module to control the brightness.
*/

#include "brightness_controller.h"
#include "rte.h"

#ifdef CONFIG_BRIGHTNESS_ADJUSTMENT_PERIOD

#define BRIGHTNESS_PERIOD_TICKS ((unsigned int)((CONFIG_BRIGHTNESS_ADJUSTMENT_PERIOD*1000) / BRIGHTNESS_TASK_PERIOD))

brightness_t periodicBrightnessAdjustment(BrightnessAdjustmentData* data) {
    brightness_t brightnessValue = 0;

    if (data->timeCounter < data->halfPeriod) {
        // Ramp up linearly from min to max
        brightnessValue = data->minBrightness + ((data->maxBrightness - data->minBrightness) * data->timeCounter) / data->halfPeriod;
    }
    else {
        // Ramp down linearly from max to min
        brightnessValue = data->maxBrightness - ((data->maxBrightness - data->minBrightness) * (data->timeCounter - data->halfPeriod)) / data->halfPeriod;
    }

    // Increment and reset timeCounter based on the period
    data->timeCounter = (data->timeCounter + 1) % data->period;

    return brightnessValue;
}

#else

/*!
* @rst
*
* .. impl:: Brightness Calculation
*    :id: SWIMPL_BC-001
*    :implements: SWDD_BC-001, SWDD_BC-002
* @endrst
*/
brightness_t manualBrightnessAdjustment(void) {
    percentage_t mainKnobValue = RteGetMainKnobValue();
    brightness_t brightnessValue = 0;

    if (mainKnobValue == 0) {
        brightnessValue = 0;
    }
    else {
        brightnessValue = (mainKnobValue * 255) / 100;
    }
    return brightnessValue;
}

#endif

void brightnessController(void) {
    brightness_t brightnessValue = 0;
#ifdef CONFIG_BRIGHTNESS_ADJUSTMENT_AUTOMATIC
    static BrightnessAdjustmentData data = {
        .timeCounter = 0,
        .period = BRIGHTNESS_PERIOD_TICKS,
        .halfPeriod = BRIGHTNESS_PERIOD_TICKS / 2,
        .maxBrightness = 200,
        .minBrightness = 50
    };

    brightnessValue = periodicBrightnessAdjustment(&data);
    RteSetBrightnessAdjustmentCounter(data.timeCounter);
#elif defined(CONFIG_BRIGHTNESS_ADJUSTMENT_MANUALLY)
    brightnessValue = manualBrightnessAdjustment();
#endif
    RteSetBrightnessValue(brightnessValue);
}
