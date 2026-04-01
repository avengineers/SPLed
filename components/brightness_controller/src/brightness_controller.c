/**
 * @file brightness_controller.c
 * @brief Module to control the brightness.
 */

#include "brightness_controller.h"

#ifdef CONFIG_BRIGHTNESS_ADJUSTMENT_AUTOMATIC

/** @brief Calculate the number of ticks for the brightness adjustment period. */
#define BRIGHTNESS_PERIOD_TICKS ((uint32_t)((CONFIG_BRIGHTNESS_ADJUSTMENT_PERIOD * 1000) / CONFIG_OS_TASK_PERIOD))

/**
 * @rst
 * .. impl:: Periodic Brightness Adjustment
 *    :id: SWIMPL_BC-001
 *    :implements: SWDD_BC-100, SWDD_BC-102
 *    :fulfills: REQ_46, REQ_48
 * @endrst
 */
SPLE_TESTABLE_STATIC brightness_t periodicBrightnessAdjustment(BrightnessAdjustmentData *const data)
{
    brightness_t brightnessValue = 0;

    if (data->ticksCounter < data->halfPeriod)
    {
        // Ramp up linearly from min to max
        brightnessValue = (brightness_t)(data->minBrightness + ((((uint32_t)data->maxBrightness - (uint32_t)data->minBrightness) * data->ticksCounter) / data->halfPeriod));
    }
    else
    {
        // Ramp down linearly from max to min
        brightnessValue = (brightness_t)(data->maxBrightness - ((((uint32_t)data->maxBrightness - (uint32_t)data->minBrightness) * (data->ticksCounter - data->halfPeriod)) / data->halfPeriod));
    }

    // Increment and reset ticksCounter based on the period
    data->ticksCounter = (data->ticksCounter + 1) % data->period;

    return brightnessValue;
}

#else /* CONFIG_BRIGHTNESS_ADJUSTMENT_AUTOMATIC */

/**
 * @rst
 * .. impl:: Manual Brightness Adjustment
 *    :id: SWIMPL_BC-002
 *    :implements: SWDD_BC-100, SWDD_BC-101, SWDD_BC-201
 *    :fulfills: REQ_46, REQ_47
 * @endrst
 */
static brightness_t manualBrightnessAdjustment(void)
{
    const percentage_t mainKnobValue = RteGetMainKnobValue();
    brightness_t brightnessValue = 0;

    if (mainKnobValue == 0)
    {
        brightnessValue = 0;
    }
    else
    {
        brightnessValue = (brightness_t)(((uint32_t)mainKnobValue * 255) / 100);
    }
    return brightnessValue;
}

#endif /* CONFIG_BRIGHTNESS_ADJUSTMENT_AUTOMATIC */

/**
 * @rst
 * .. impl:: Brightness Controller runnable
 *    :id: SWIMPL_BC-003
 *    :implements: SWDD_BC-200, SWDD_BC-202, SWDD_BC-203
 *    :fulfills: REQ_46, REQ_47, REQ_48
 * @endrst
 */
void brightnessController(void)
{
    brightness_t brightnessValue = 0;
#ifdef CONFIG_BRIGHTNESS_ADJUSTMENT_AUTOMATIC
    static BrightnessAdjustmentData data = {
        .ticksCounter = 0,
        .period = BRIGHTNESS_PERIOD_TICKS,
        .halfPeriod = BRIGHTNESS_PERIOD_TICKS / 2,
        .maxBrightness = 200,
        .minBrightness = 50};

    brightnessValue = periodicBrightnessAdjustment(&data);
    RteSetBrightnessAdjustmentCounter(data.ticksCounter);
#elif defined(CONFIG_BRIGHTNESS_ADJUSTMENT_MANUAL)
    brightnessValue = manualBrightnessAdjustment();
#endif
    RteSetBrightnessValue(brightnessValue);
}
