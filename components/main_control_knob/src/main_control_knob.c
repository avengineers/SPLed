/**
 * @file
 * @brief Implementation of the main control knob module
 */

#include "main_control_knob.h"
#include "rte.h"

#define KNOB_UPDATE_INCREMENT 2u
#define MIN_KNOB_VALUE 0u
#define MAX_KNOB_VALUE 100u

/**
 * @rst
 * .. impl:: Main Control Knob's main function
 *    :id: SWIMPL_MCK-001
 *    :implements: SWDD_MCK-100, SWDD_MCK-101, SWDD_MCK-200, SWDD_MCK-201, SWDD_MCK-202, SWDD_MCK-203
 *    :fulfills: REQ_50
 * @endrst
 */
void mainControlKnob(void)
{
    percentage_t currentValue = RteGetMainKnobValue();
    if (RteIsKeyPressed(CONTROL_KEY_UP) == TRUE)
    {
        // Increase knob's percentage value by KNOB_UPDATE_INCREMENT with a maximum of 100.
        if (currentValue < (MAX_KNOB_VALUE - KNOB_UPDATE_INCREMENT))
        {
            currentValue += KNOB_UPDATE_INCREMENT;
        }
        else
        {
            currentValue = MAX_KNOB_VALUE;
        }
    }
    else
    {
        if (RteIsKeyPressed(CONTROL_KEY_DOWN) == TRUE)
        {
            // Decrease knob's percentage value by KNOB_UPDATE_INCREMENT with a minimum of 0.
            if (currentValue > KNOB_UPDATE_INCREMENT)
            {
                currentValue -= KNOB_UPDATE_INCREMENT;
            }
            else
            {
                currentValue = MIN_KNOB_VALUE;
            }
        }
    }
    RteSetMainKnobValue(currentValue);
}
