/**
 * @file keyboard_interface.c
 * @brief Module to interface with keyboard and detect specific key presses.
 */

#include "keyboard_interface.h"
#include "rte.h"
static boolean previousState = FALSE; /**< Previous state of the TARGET_KEY. */

/*!
 * @rst
 *
 * .. impl:: Power signal processing
 *    :id: SWIMPL_PSP-001
 *    :implements: SWDD_PSP-001, SWDD_PSP-002, SWDD_PSP-003
 * @endrst
 * 
 * @brief Interface with the keyboard and detect state changes of the TARGET_KEY.
 * 
 * This function should be called periodically to detect state changes of the TARGET_KEY.
 * If the TARGET_KEY was just pressed, RteSetPowerKeyPressed will be called with TRUE.
 * If the TARGET_KEY was just released, RteSetPowerKeyPressed will be called with FALSE.
 * 
 * Note: The frequency of calling this function will determine system's responsiveness to key presses.

*/
void keyboardInterface(){
    boolean currentState = RteIsKeyPressed(TARGET_KEY);

    if (currentState && !previousState) {
        // The key was just pressed
        RteSetPowerKeyPressed(TRUE);
    } else if (!currentState && previousState) {
        // The key was just released
        RteSetPowerKeyPressed(FALSE);
    }

    // Update the previous state
    previousState = currentState;
}
