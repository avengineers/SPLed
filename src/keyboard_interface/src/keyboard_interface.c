/**
 * @file keyboard_interface.c
 * @brief Module to interface with keyboard and debounce specific key presses/releases.
 */

#include "keyboard_interface.h"
#include "rte.h"

/**
 * @enum KeyState
 * @brief States for the debouncing state machine.
 */
typedef enum {
    INIT,     /**< Initial state, waiting for debounce timers. */
    PRESSED,  /**< Key has been debounced as pressed. */
    RELEASED  /**< Key has been debounced as released. */
} KeyState;

static KeyState currentState = INIT; /**< Current state of the debouncing state machine. */
static unsigned int pressCounter = 0; /**< Counter for key presses. */
static unsigned int releaseCounter = 0; /**< Counter for key releases. */

/**
 * @brief Debounces key presses and releases.
 * 
 * This function utilizes a state machine to debounce key presses and releases.
 * It must be called periodically to process the debouncing. The behavior is controlled
 * by two configuration parameters: CFG_DEBOUNCE_PRESS and CFG_DEBOUNCE_RELEASE which determine 
 * how many consecutive calls with the key pressed/released are required to acknowledge the 
 * state transition.
 */
void keyboardInterface() {
    boolean powerKeyPressed = FALSE;
    boolean keyStatus = RteIsKeyPressed(TARGET_KEY);

    // Update the counters
    if (keyStatus) {
        pressCounter++;
        releaseCounter = 0; // reset release counter if key is pressed
    } else {
        releaseCounter++;
        pressCounter = 0;  // reset press counter if key is released
    }


    switch (currentState) {
        case INIT:
            if (pressCounter >= CFG_DEBOUNCE_PRESS) {
                powerKeyPressed = TRUE;
                currentState = PRESSED;
                releaseCounter = 0; // reset the counter after transition
                pressCounter = 0; // reset the counter after transition
            } else if (releaseCounter >= CFG_DEBOUNCE_RELEASE) {
                currentState = RELEASED;
                releaseCounter = 0; // reset the counter after transition
                pressCounter = 0; // reset the counter after transition
            }
            break;

        case PRESSED:
            if (releaseCounter >= CFG_DEBOUNCE_RELEASE) {
                currentState = RELEASED;
                releaseCounter = 0; // reset the counter after transition
            }
            pressCounter = 0; // reset the counter after transition
            break;

        case RELEASED:
            if (pressCounter >= CFG_DEBOUNCE_PRESS) {
                powerKeyPressed = TRUE;
                currentState = PRESSED;
                pressCounter = 0; // reset the counter after transition
            }
            releaseCounter = 0; // reset the counter after transition
            break;
    }

    RteSetPowerKeyPressed(powerKeyPressed);
}
