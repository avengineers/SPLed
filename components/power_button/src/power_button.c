/**
 * @file power_button.c
 * @brief Module to interface with power button keyboard
 */

#include "power_button.h"
#include "rte.h"

/**
 * @enum KeyState
 * @brief States for the debouncing state machine.
 */
typedef enum
{
    INIT,    /**< Initial state, waiting for debounce timers. */
    PRESSED, /**< Key has been debounced as pressed. */
    RELEASED /**< Key has been debounced as released. */
} KeyState;

static KeyState currentState = INIT; /**< Current state of the debouncing state machine. */
static uint8_t pressCounter = 0;     /**< Counter for key presses. */
static uint8_t releaseCounter = 0;   /**< Counter for key releases. */

void powerButtonInit(void)
{
    currentState = INIT;
    pressCounter = 0;
    releaseCounter = 0;
}

/**
 * @brief Debounces key presses and releases.
 *
 * This function utilizes a state machine to debounce key presses and releases.
 * It must be called periodically to process the debouncing. The behavior is controlled
 * by two configuration parameters: POWER_BUTTON_PRESS_DEBOUNCE and POWER_BUTTON_RELEASE_DEBOUNCE which determine
 * how many consecutive calls with the key pressed/released are required to acknowledge the
 * state transition.
 */
void powerButton(void)
{
    bool_t powerKeyPressed = FALSE;
    const bool_t keyStatus = RteIsKeyPressed(POWER_BUTTON_KEY);

    // Update the counters
    if (keyStatus == TRUE)
    {
        pressCounter++;
        releaseCounter = 0; // reset release counter if key is pressed
    }
    else
    {
        releaseCounter++;
        pressCounter = 0; // reset press counter if key is released
    }

    switch (currentState)
    {
    case INIT:
        if (pressCounter >= POWER_BUTTON_PRESS_DEBOUNCE)
        {
            powerKeyPressed = TRUE;
            currentState = PRESSED;
            releaseCounter = 0; // reset the counter after transition
            pressCounter = 0;   // reset the counter after transition
        }
        else
        {
            if (releaseCounter >= POWER_BUTTON_RELEASE_DEBOUNCE)
            {
                currentState = RELEASED;
                releaseCounter = 0; // reset the counter after transition
                pressCounter = 0;   // reset the counter after transition
            }
        }
        break;

    case PRESSED:
        if (releaseCounter >= POWER_BUTTON_RELEASE_DEBOUNCE)
        {
            currentState = RELEASED;
            releaseCounter = 0; // reset the counter after transition
        }
        pressCounter = 0; // reset the counter after transition
        break;

    default: /* RELEASED */
        if (pressCounter >= POWER_BUTTON_PRESS_DEBOUNCE)
        {
            powerKeyPressed = TRUE;
            currentState = PRESSED;
            pressCounter = 0; // reset the counter after transition
        }
        releaseCounter = 0; // reset the counter after transition
        break;
    }

    RteSetPowerKeyPressedEvent(powerKeyPressed);
}
