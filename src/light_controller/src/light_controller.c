/**
 * @file light_controller.c
 * @brief Module to control light based on power state.
 */

#include "light_controller.h"
#include "rte.h"

 /**
  * * @rst
  * .. impl:: Light state
  *    :id: SWIMPL_LC-001
  *    :implements: SWDD_LC-002
  * @endrst
  *
  * @enum LightState
  * @brief Represents the states of the light.
  */
typedef enum {
    LIGHT_OFF,  /**< Represents a state where the light is turned off. */
    LIGHT_ON    /**< Represents a state where the light is turned on with a specific color. */
} LightState;

static LightState currentLightState = LIGHT_OFF;  /**< Current state of the light. */
static int blinkCounter = 0;
static boolean blinkState = FALSE;

/**
 * * @rst
 * .. impl:: Turn light off
 *    :id: SWIMPL_LC-002
 *    :implements: SWDD_LC-003
 * @endrst
 */
static void turnLightOff(void) {
    RGBColor color = {
        .red = 0,
        .green = 0,
        .blue = 0
    };
    blinkState = FALSE;
    RteSetLightValue(color);
}

/**
 * * @rst
 * .. impl:: Turn light on
 *    :id: SWIMPL_LC-003
 *    :implements: SWDD_LC-003
 * @endrst
 */
static void turnLightOn(void) {
    RGBColor color = {
        .red = 0,
        .green = 128,
        .blue = 55
    };
    blinkState = TRUE;
    RteSetLightValue(color);
}

/*!
* @rst
*
* .. impl:: Calculate blink period
*    :id: SWIMPL_LC-004
*    :implements: SWDD_LC-002
* @endrst
*/
static unsigned int calculateBlinkPeriod(percentage_t mainKnobValue) {
    // Calculate blink period based on main knob value
    unsigned int blinkPeriod = 100 - (mainKnobValue); // Adjust this formula as needed

    // Ensure there's a minimum blink period
    blinkPeriod = (blinkPeriod > 10) ? blinkPeriod : 10; // Adjust the minimum period as needed

    return blinkPeriod;
}

/**
 * @brief Controls the light behavior based on the power state.
 *
 * Uses a state machine to determine the light's color. The light color is only updated
 * when there's a change in the power state to ensure no redundant updates.
 */
void lightController(void) {

    PowerState powerState = RteGetPowerState();
    percentage_t mainKnobValue = RteGetMainKnobValue();
    unsigned int blinkPeriod = calculateBlinkPeriod(mainKnobValue);

#if LOGGING_ENABLED
    RteLoggerPrintToConsole(LOG_LEVEL_DEBUG, "Light controller: power state = %d, main knob value = %d, blink period = %d", powerState, mainKnobValue, blinkPeriod);
#endif

    switch (currentLightState) {
    case LIGHT_OFF:
        blinkCounter = 0;
        if (powerState != POWER_STATE_OFF) {
            turnLightOn();
            currentLightState = LIGHT_ON;
        }
        break;

    case LIGHT_ON:
        if (powerState == POWER_STATE_OFF) {
            turnLightOff();
            currentLightState = LIGHT_OFF;
        }
        else {
            blinkCounter++;
            if (blinkCounter >= blinkPeriod) {
                // Toggle the LED state
                if (blinkState == TRUE) {
                    turnLightOff();
                }
                else {
                    turnLightOn();
                }
                blinkCounter = 0;
            }
        }
        break;
    }
}
