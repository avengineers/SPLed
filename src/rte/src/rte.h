/**
 * @file rte.h
 * @brief Runtime environment (RTE) interface for power and light functionalities.
 */

#ifndef __rte_h__
#define __rte_h__

/** @brief Boolean type definition. */
typedef unsigned char boolean;

/** @brief Macro for boolean TRUE. */
#define TRUE 1

/** @brief Macro for boolean FALSE. */
#define FALSE 0

/**
 * @brief Enumerated type for power states.
 */
typedef enum {
    POWER_STATE_OFF = 0, /**< Power is turned off. */
    POWER_STATE_ON  = 1  /**< Power is turned on. */
} PowerState;

/**
 * @brief Set the current power state.
 * 
 * @param state The desired power state.
 */
void RteSetPowerState(PowerState state);

/**
 * @brief Retrieve the current power state.
 * 
 * @return The current power state.
 */
PowerState RteGetPowerState(void);

/**
 * @brief Get the state of the power key being pressed.
 * 
 * @return TRUE if the power key was pressed, FALSE otherwise.
 */
boolean RteGetPowerKeyPressed(void);

/**
 * @brief Set the state of the power key being pressed.
 * 
 * @param value The state to set for the power key press.
 */
void RteSetPowerKeyPressed(boolean value);

/**
 * @brief Set the value of the light.
 * 
 * @param value An integer representing the desired light value.
 */
void RteSetLightValue(int value);

/**
 * @brief Retrieve the current light value.
 * 
 * @return The current light value.
 */
int RteGetLightValue(void);

/**
 * @brief Check if the given key is currently pressed.
 * 
 * This function checks the most significant bit of the return value from GetAsyncKeyState 
 * to determine if the key is currently pressed or not.
 * 
 * @param key The virtual-key code of the key to be checked.
 * @return TRUE if the key is pressed, FALSE otherwise.
 */
boolean RteIsKeyPressed(int key);

#endif // __rte_h__
