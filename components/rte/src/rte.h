/**
 * @file rte.h
 * @brief Runtime environment (RTE) interface for power and light functionalities.
 */

#ifndef RTE_H
#define RTE_H

#include "autoconf.h"
#include "platform_types.h"

/**
 * @brief Key code definition for Arrow Up
 */
#define CONTROL_KEY_UP 0x26
/**
 * @brief Key code definition for Arrow Down
 */
#define CONTROL_KEY_DOWN 0x28

/**
 * @brief Key code definition for Power Button 'P'
 */
#define POWER_BUTTON_KEY (int32_t)'P'

/**
 * @brief Enumerated type for power states.
 */
typedef enum
{
    POWER_STATE_OFF = 0, /**< Power is turned off. */
    POWER_STATE_ON = 1   /**< Power is turned on. */
} PowerState;

/**
 * @brief Struct representing an RGB color.
 */
typedef struct
{ /* polyspace CERT-C:DCL12-C [Justified:Low] "No difference between getter/setter usage and direct struct member access in this context" polyspace MISRA-C3:D4.8 [Justified:Low] "Type usage independent of pointer usage" */

    uint8_t rgbRedValue;   /**< The red component of the color. */
    uint8_t rgbGreenValue; /**< The green component of the color. */
    uint8_t rgbBlueValue;  /**< The blue component of the color. */
} RGBColor;

/**
 * @typedef Percentage
 * @brief Represents a positive percentage value ranging from 0 to 100.
 */
typedef uint8_t percentage_t;

/**
 * @typedef Brightness
 * @brief Represents a brightness value ranging from 0 to 255.
 */
typedef uint8_t brightness_t;

/**
 * @brief Set the current power state.
 *
 * @param state The desired power state.
 */
void RteSetPowerState(PowerState currentPowerState);

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
bool_t RteGetPowerKeyPressedEvent(void);

/**
 * @brief Set the state of the power key being pressed.
 *
 * @param value The state to set for the power key press.
 */
void RteSetPowerKeyPressedEvent(bool_t powerKeyPressedEvent);

/**
 * @brief Sets the value of the RGB light.
 *
 * This function sets the value of the RGB light to the specified color.
 *
 * @param lightValue The RGB color to set the light to.
 */
void RteSetLightValue(const RGBColor lightValue);

/**
 * @brief Gets the current value of the RGB light.
 *
 * This function retrieves the current value of the RGB light and stores it in the provided RGBColor pointer.
 *
 * @param value A pointer to an RGBColor struct to store the current light value in.
 */
void RteGetLightValue(RGBColor *const lightValue);

/**
 * @brief Check if the given key is currently pressed.
 *
 * This function checks the most significant bit of the return value from GetAsyncKeyState
 * to determine if the key is currently pressed or not.
 *
 * @param key The virtual-key code of the key to be checked.
 * @return TRUE if the key is pressed, FALSE otherwise.
 */
bool_t RteIsKeyPressed(int32_t keyCode);

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
void RteSetMainKnobValue(percentage_t mainKnobValue);

/**
 * @brief Gets the value of the main knob.
 *
 * This function retrieves the current percentage value of the main knob.
 *
 * @return The percentage value of the main knob, between 0 and 100 (inclusive).
 */
percentage_t RteGetMainKnobValue(void);

/**
 * @brief Sets the value of the brightness.
 *
 * This function sets the value of the brightness. The value is based on
 * the main knob value (percentage) and must be between 0 and 255 (inclusive).
 *
 * @param[in] value value to set. Must be between 0 and 255.
 *
 * @note Values greater than 255 will be clamped to 255.
 */
void RteSetBrightnessValue(brightness_t brightnessValue);

/**
 * @brief Gets the value of the brightness.
 *
 * This function retrieves the current brightness value of the light.
 *
 * @return The percentage value of the brightness, between 0 and 255 (inclusive).
 */
brightness_t RteGetBrightnessValue(void);

#ifdef CONFIG_BRIGHTNESS_ADJUSTMENT_PERIOD
/**
 * @brief Sets the brightness adjustment counter.
 *
 * This function sets the brightness adjustment counter to the specified value.
 *
 * @param[in] counter The value to set for the brightness adjustment counter.
 */
void RteSetBrightnessAdjustmentCounter(uint32_t counter);

/**
 * @brief Gets the brightness adjustment counter.
 *
 * This function retrieves the current value of the brightness adjustment counter.
 *
 * @param[out] counter A pointer to an integer where the current brightness adjustment counter will be stored.
 */
void RteGetBrightnessAdjustmentCounter(uint32_t *const counter);
#endif // CONFIG_BRIGHTNESS_ADJUSTMENT_PERIOD

/**
 * @brief Sets the off course state.
 *
 * @param value TRUE to indicate off course, FALSE otherwise.
 */
void RteSetOffCourse(bool_t offCourse);

/**
 * @brief Retrieves whether the system is off course.
 *
 * @return TRUE if off course, FALSE otherwise.
 */
void RteGetOffCourse(bool_t *const offCourse);

/**
 * @brief Sets whether an abort has been commanded.
 *
 * @param commanded TRUE to indicate abort commanded, FALSE otherwise.
 */
void RteSetAbortCommanded(bool_t commanded);

/**
 * @brief Retrieves whether an abort has been commanded.
 *
 * @return TRUE if abort is commanded, FALSE otherwise.
 */
bool_t RteGetAbortCommanded(void);

/**
 * @brief Sets whether the abort command is valid.
 */
void RteSetValidAbortCommand(bool_t valid);

/**
 * @brief Retrieves whether the abort command is valid.
 *
 * @return TRUE if abort command is valid, FALSE otherwise.
 */
bool_t RteGetValidAbortCommand(void);

/**
 * @brief Sets the SelfDestruct state.
 *
 * @param state TRUE to trigger SelfDestruct, FALSE otherwise.
 */
void RteSetSelfDestructState(bool_t selfDestructState);

/**
 * @brief Retrieves the SelfDestruct state.
 *
 * @return TRUE if SelfDestruct is triggered, FALSE otherwise.
 */
bool_t RteGetSelfDestructState(void);

#ifdef CONFIG_AUTO_OFF
/**
 * @brief Get the state of the auto off event.
 *
 * @return TRUE if the auto off event occurred, FALSE otherwise.
 */
bool_t RteGetAutoOffState(void);
#endif // CONFIG_AUTO_OFF

#ifdef CONFIG_AUTO_OFF
/**
 * @brief Set the state of the auto off event.
 *
 * @param value The state to set for the auto off event.
 */
void RteSetAutoOffState(bool_t state);
#endif // CONFIG_AUTO_OFF

#endif // RTE_H
