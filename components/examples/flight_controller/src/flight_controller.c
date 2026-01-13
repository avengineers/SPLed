/** @file */
#include "flight_controller.h"

// Example function to demonstrate the use of MC/DC (code coverage metric).
// See this youtube video for more information:
// https://www.youtube.com/watch?v=k0_PF8MtEEo

/**
 * @rst
 * .. impl:: Abort decision logic
 *    :id: SWIMPL_FC-001
 *    :implements: SWDD_FC-100, SWDD_FC-101, SWDD_FC-102
 * @endrst
 */
SPLE_TESTABLE_STATIC bool_t CheckAbort(bool_t abort_commanded, bool_t valid_abort_command, bool_t off_course)
{
    bool_t result;
    if (((abort_commanded == TRUE) && (valid_abort_command == TRUE)) || (off_course == TRUE))
    {
        result = TRUE;
    }
    else
    {
        result = FALSE;
    }
    return result;
}

/**
 * @rst
 * .. impl:: Flight Controller's main runnable
 *    :id: SWIMPL_FC-002
 *    :implements: SWDD_FC-103, SWDD_FC-200, SWDD_FC-201, SWDD_FC-202, SWDD_FC-203, SWDD_FC-204
 * @endrst
 */
void flightController(void)
{
    /* Get all relevant signals */
    const bool_t abort_commanded = RteGetAbortCommanded();
    const bool_t valid_abort_command = RteGetValidAbortCommand();
    bool_t off_course;
    RteGetOffCourse(&off_course);

    /* Determine if we should abort the mission */
    const bool_t abort_decision = CheckAbort(abort_commanded, valid_abort_command, off_course);

    /* Set self-destruct state based on abort decision */
    RteSetSelfDestructState(abort_decision);
}
