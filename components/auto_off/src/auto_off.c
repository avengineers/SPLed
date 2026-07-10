/**
 * @file auto_off.c
 * @brief Auto off functionality component
 */

#include "auto_off.h"
#include "autoconf.h"
#include "rte.h"

SPLE_TESTABLE_STATIC uint32_t autoOffTimer = 0; /* Auto off timer in milliseconds */

/**
 * @brief Initialize auto off functionality
 */
void autoOffInit(void)
{
    autoOffTimer = 0;
}

/**
 * @brief Auto off runnable function
 */
void autoOff(void)
{
    // Check if any key (power, up, down is pressed)
    if (
        (RteIsKeyPressed(POWER_BUTTON_KEY) == TRUE) ||
        (RteIsKeyPressed(CONTROL_KEY_UP) == TRUE) ||
        (RteIsKeyPressed(CONTROL_KEY_DOWN) == TRUE))
    {
        // Reset auto off timer to configured value
        autoOffTimer = (CONFIG_AUTO_OFF_PERIOD_SECONDS * 1000) + CONFIG_OS_TASK_PERIOD; // Convert seconds to milliseconds
    }

    if (autoOffTimer > CONFIG_OS_TASK_PERIOD)
    {
        autoOffTimer -= CONFIG_OS_TASK_PERIOD; // Decrease by task period in milliseconds
        RteSetAutoOffState(FALSE);             // Clear auto off state
    }
    else
    {
        autoOffTimer = 0;         // Ensure timer doesn't go negative
        RteSetAutoOffState(TRUE); // Trigger auto off state
    }
}
