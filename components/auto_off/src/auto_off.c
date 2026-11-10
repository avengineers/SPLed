/**
 * @file auto_off.c
 * @brief Auto off component: decides when the light has been idle long enough to be switched off.
 */

#include "auto_off.h"
#include "autoconf.h"
#include "rte.h"

/** The configured auto off period, in the millisecond unit the timer uses. */
#define AUTO_OFF_PERIOD_MS ((uint32_t)CONFIG_AUTO_OFF_PERIOD_SECONDS * 1000u)

/** One execution of this component, in milliseconds. */
#define AUTO_OFF_TICK_MS ((uint32_t)CONFIG_OS_TASK_PERIOD)

/** Time left before the auto off state is reported, in milliseconds. */
SPLE_TESTABLE_STATIC uint32_t autoOffTimer = 0u;

/**
 * @brief Report whether the user is pressing one of the monitored keys.
 *
 * Internal to the runnable below, which carries the traceability for it. The source doc
 * extractor does not see a function whose declaration starts with SPLE_TESTABLE_STATIC, so an
 * impl block here would not reach the component report.
 *
 * @return TRUE if the power button, the up key or the down key is pressed, FALSE otherwise.
 */
SPLE_TESTABLE_STATIC bool_t autoOffIsUserActive(void)
{
    bool_t isUserActive;

    if ((RteIsKeyPressed(POWER_BUTTON_KEY) == TRUE) ||
        (RteIsKeyPressed(CONTROL_KEY_UP) == TRUE) ||
        (RteIsKeyPressed(CONTROL_KEY_DOWN) == TRUE))
    {
        isUserActive = TRUE;
    }
    else
    {
        isUserActive = FALSE;
    }

    return isUserActive;
}

/**
 * @rst
 * .. impl:: Auto Off's main runnable
 *    :id: SWIMPL_AO-001
 *    :implements: SWDD_AO-110, SWDD_AO-111, SWDD_AO-112, SWDD_AO-113, SWDD_AO-114,
 *                 SWDD_AO-115, SWDD_AO-210, SWDD_AO-211, SWDD_AO-212, SWDD_AO-310,
 *                 SWDD_AO-311
 *    :fulfills: REQ_39, REQ_40
 * @endrst
 */
void autoOff(void)
{
    if (autoOffIsUserActive() == TRUE)
    {
        /* Any key press starts the period again. */
        autoOffTimer = AUTO_OFF_PERIOD_MS;
    }
    else if (autoOffTimer >= AUTO_OFF_TICK_MS)
    {
        autoOffTimer -= AUTO_OFF_TICK_MS;
    }
    else
    {
        /* Less than one task period left: stop at zero instead of wrapping around. */
        autoOffTimer = 0u;
    }

    if (autoOffTimer == 0u)
    {
        RteSetAutoOffState(TRUE);
    }
    else
    {
        RteSetAutoOffState(FALSE);
    }
}
