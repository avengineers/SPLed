/**
 * @file light_controller.c
 * @brief Module to control light based on power state.
 */

#include "light_controller.h"
#include "rte.h"

 /**
  * @enum LightState
  * @brief Represents the states of the light.
  */
typedef enum {
    LIGHT_OFF,  /**< Represents a state where the light is turned off. */
    LIGHT_ON    /**< Represents a state where the light is turned on with a specific color. */
} LightState;

static LightState currentLightState = LIGHT_OFF;  /**< Current state of the light. */

/**
 * @brief Controls the light behavior based on the power state.
 *
 * Uses a state machine to determine the light's color. The light color is only updated
 * when there's a change in the power state to ensure no redundant updates.
 */
void lightController(void) {
    // Fetch the power state
    PowerState powerState = RteGetPowerState();

    // Fetch the main knob value
    percentage_t mainKnobValue = RteGetMainKnobValue();

    // Calculate blink speed based on main knob value
    int blinkSpeed = 100 - mainKnobValue; // Adjust this formula as needed

    // Ensure there's a minimum blink speed
    blinkSpeed = (blinkSpeed > 10) ? blinkSpeed : 10; // Adjust the minimum speed as needed

    switch (currentLightState) {
    case LIGHT_OFF:
        if (powerState != POWER_STATE_OFF) {
            // Transition action: turn the light on with specific color
            RGBColor color = {
                .red = 0,
                .green = 128,
                .blue = 55
            };
            RteSetLightValue(color);

            // Transition to LIGHT_ON state
            currentLightState = LIGHT_ON;
        }
        break;

    case LIGHT_ON:
        if (powerState == POWER_STATE_OFF) {
            // Transition action: turn the light off
            RGBColor color = {
                .red = 0,
                .green = 0,
                .blue = 0
            };
            RteSetLightValue(color);

            // Transition to LIGHT_OFF state
            currentLightState = LIGHT_OFF;
        }
        else {
            // Implement blinking based on blinkSpeed
            static int blinkCounter = 0;
            blinkCounter++;
            if (blinkCounter >= blinkSpeed) {
                // Toggle the LED state
                if (currentLightState == LIGHT_ON) {
                    RGBColor color = {
                        .red = 0,
                        .green = 0,
                        .blue = 0
                    };
                    RteSetLightValue(color);
                    currentLightState = LIGHT_OFF;
                }
                else {
                    RGBColor color = {
                        .red = 0,
                        .green = 128,
                        .blue = 55
                    };
                    RteSetLightValue(color);
                    currentLightState = LIGHT_ON;
                }
                blinkCounter = 0;  // Reset the counter
            }
        }
        break;
    }
}
