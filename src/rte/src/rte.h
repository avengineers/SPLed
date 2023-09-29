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

#define LOGGING_ENABLED 0

#if LOGGING_ENABLED
/**
 * @brief Enumerated type for log levels.
 */
typedef enum {
    LOG_LEVEL_DEBUG = 0,
    LOG_LEVEL_INFO = 1,
    LOG_LEVEL_WARNING = 2,
    LOG_LEVEL_ERROR = 3
} LogLevel;
#endif

/**
 * @brief Enumerated type for power states.
 */
typedef enum {
    POWER_STATE_OFF = 0, /**< Power is turned off. */
    POWER_STATE_ON = 1  /**< Power is turned on. */
} PowerState;

/**
 * @brief Struct representing an RGB color.
 */
typedef struct {
    int red; /**< The red component of the color. */
    int green; /**< The green component of the color. */
    int blue; /**< The blue component of the color. */
} RGBColor;

/**
 * @typedef Percentage
 * @brief Represents a positive percentage value ranging from 0 to 100.
 */
typedef unsigned int percentage_t;

/**
 * @brief Represents a blink speed
 */
extern int blinkSpeed;


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
 * @brief Sets the value of the RGB light.
 *
 * This function sets the value of the RGB light to the specified color.
 *
 * @param value The RGB color to set the light to.
 */
void RteSetLightValue(RGBColor value);

/**
 * @brief Gets the current value of the RGB light.
 *
 * This function retrieves the current value of the RGB light and stores it in the provided RGBColor pointer.
 *
 * @param value A pointer to an RGBColor struct to store the current light value in.
 */
void RteGetLightValue(RGBColor* value);

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

/**
 * @brief Sets the value of the main knob.
 *
 * This function sets the value of the main knob. The value should
 * represent a percentage and thus must be between 0 and 100 (inclusive).
 *
 * @param[in] value Percentage value to set. Must be between 0 and 100.
 *
 * @note Values greater than 100 will be clamped to 100.
 */
void RteSetMainKnobValue(percentage_t value);

/**
 * @brief Gets the value of the main knob.
 *
 * This function retrieves the current percentage value of the main knob.
 *
 * @return The percentage value of the main knob, between 0 and 100 (inclusive).
 */
percentage_t RteGetMainKnobValue(void);

#if LOGGING_ENABLED
/**
 * @brief Prints a message to the console.
 *
 * This function prints a message to the console with the given log level.
 *
 * @param[in] level The log level of the message.
 * @param[in] message The message to print.
 */
void RteLoggerPrintToConsole(LogLevel level, const char* message, ...);
#endif

#endif // __rte_h__
