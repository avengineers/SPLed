/**
 * @file power_signal_processing.c
 * @brief Module responsible for processing power signals based on key presses.
 */

#include "power_signal_processing.h"
#include "rte.h"

/**
 * @rst
 *
 * .. impl:: Power signal processing
 *    :id: SWIMPL_PSP-001
 *    :implements: SWDD_PSP-001, SWDD_PSP-002, SWDD_PSP-003
 *    :fulfills: REQ_38
 * @endrst
 */
void powerSignalProcessing(void)
{
    // Check if "P" key was pressed
    if (RteGetPowerKeyPressedEvent() == TRUE)
    {
        // Toggle power state
        if (RteGetPowerState() == POWER_STATE_OFF)
        {
            RteSetPowerState(POWER_STATE_ON);
        }
        else
        {
            RteSetPowerState(POWER_STATE_OFF);
        }
    }
#ifdef CONFIG_AUTO_OFF
    else
    {
        if (RteGetAutoOffState() == TRUE)
        {
            // If auto off event occurred, turn off power
            RteSetPowerState(POWER_STATE_OFF);
        }
    }
#endif
}
