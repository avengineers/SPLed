/**
 * @file light_controller.c
 * @brief Module to control light based on power state.
 */

#include "light_controller.h"
#include "rte.h"

/**
 * @enum LightColor
 * @brief Represents the possible light colors.
 */
typedef enum
{
    COLOR_VAL_OFF,
    COLOR_VAL_GREEN,
    COLOR_VAL_BLUE,
    COLOR_VAL_RED,
    COLOR_VAL_PURPLE
} LightColor;

/**
 * @rst
 * .. impl:: Light state
 *    :id: SWIMPL_LC-001
 *    :implements: SWDD_LC-100
 *    :fulfills: REQ_42
 * @endrst
 *
 * @enum LightState
 * @brief Represents the states of the light.
 */
typedef enum
{
    LIGHT_OFF, /**< Represents a state where the light is turned off. */
    LIGHT_ON   /**< Represents a state where the light is turned on with a specific color. */
} LightState;

#ifdef CONFIG_BLINKING
static bool_t blinkState = FALSE;
#endif

static const RGBColor OFF_COLOR = {.rgbRedValue = 0, .rgbGreenValue = 0, .rgbBlueValue = 0};
static LightColor light_colors[] = {
#if defined(CONFIG_COLOR_GREEN)
    COLOR_VAL_GREEN
#elif defined(CONFIG_COLOR_BLUE)
    COLOR_VAL_BLUE
#elif defined(CONFIG_COLOR_RED)
    COLOR_VAL_RED
#elif defined(CONFIG_COLOR_PURPLE)
    COLOR_VAL_PURPLE
#else
    COLOR_VAL_OFF
#endif
#if defined(CONFIG_COLOR_1_IS_ENABLED)
    ,
#if defined(CONFIG_COLOR_1_GREEN)
    COLOR_VAL_GREEN
#elif defined(CONFIG_COLOR_1_BLUE)
    COLOR_VAL_BLUE
#elif defined(CONFIG_COLOR_1_RED)
    COLOR_VAL_RED
#elif defined(CONFIG_COLOR_1_PURPLE)
    COLOR_VAL_PURPLE
#else
    COLOR_VAL_OFF
#endif
#endif // defined(CONFIG_COLOR_1_IS_ENABLED)
};
static const uint8_t light_colors_count = (uint8_t)((uint32_t)sizeof(light_colors) / (uint32_t)sizeof(light_colors[0]));

static brightness_t getBrightnessValue(void)
{
#ifdef CONFIG_BRIGHTNESS_ADJUSTMENT_ENABLED
    /**
     * @rst
     * .. impl:: Variable brightness
     *    :id: SWIMPL_LC-005
     *    :implements: SWDD_LC-204
     * @endrst
     */
    return RteGetBrightnessValue();
#else
    return 128;
#endif
}

/**
 * @brief Converts a LightColor enum and brightness to an RGBColor struct.
 * @param colorEnum The LightColor enum value.
 * @param brightness The brightness value (0-255).
 * @return The corresponding RGBColor struct.
 */
static RGBColor getRGBColorWithBrightness(LightColor colorEnum, brightness_t brightness)
{
    RGBColor color = OFF_COLOR;
    switch (colorEnum)
    {
    case COLOR_VAL_GREEN:
        color.rgbGreenValue = brightness;
        break;
    case COLOR_VAL_BLUE:
        color.rgbBlueValue = brightness;
        break;
    case COLOR_VAL_RED:
        color.rgbRedValue = brightness;
        break;
    case COLOR_VAL_PURPLE:
        color.rgbRedValue = brightness / 2; // Assuming purple is half red, full blue
        color.rgbBlueValue = brightness;
        break;
    case COLOR_VAL_OFF:
    default:
        color = OFF_COLOR;
        break;
    }
    return color;
}

/**
 * @rst
 * .. impl:: Turn light off
 *    :id: SWIMPL_LC-002
 *    :implements: SWDD_LC-102
 *    :fulfills: REQ_43
 * @endrst
 */
static void turnLightOff(void)
{
#ifdef CONFIG_BLINKING
    blinkState = FALSE;
#endif
    RteSetLightValue(OFF_COLOR);
}

/**
 * @rst
 * .. impl:: Turn light on
 *    :id: SWIMPL_LC-003
 *    :implements: SWDD_LC-102
 *    :fulfills: REQ_43
 * @endrst
 */
static void turnLightOn(void)
{
    // Used to iterate through the light colors
    static uint8_t light_colors_index = 0;

#ifdef CONFIG_BRIGHTNESS_ADJUSTMENT_PERIOD
    // Check if the brightness adjustment counter is zero to switch colors
    uint32_t counter = 0;
    RteGetBrightnessAdjustmentCounter(&counter);
    if (counter == 0)
    {
        light_colors_index = (light_colors_index + 1) % light_colors_count;
    }
#else
    light_colors_index = 0;
#endif

    // Get the current color enum and brightness
    const LightColor currentColorEnum = light_colors[light_colors_index];
    const brightness_t currentBrightness = getBrightnessValue();

    // Convert to RGBColor and set the light value
    const RGBColor color = getRGBColorWithBrightness(currentColorEnum, currentBrightness);

#ifdef CONFIG_BLINKING
    blinkState = TRUE;
#endif
    RteSetLightValue(color);
}

#ifdef CONFIG_BLINKING
/**
 * @rst
 *
 * .. impl:: Calculate blink period
 *    :id: SWIMPL_LC-004
 *    :implements: SWDD_LC-101
 *    :fulfills: REQ_44
 * @endrst
 */
SPLE_TESTABLE_STATIC percentage_t calculateBlinkPeriod(percentage_t mainKnobValue)
{
    // Calculate blink period based on main knob value
    percentage_t blinkPeriod = (percentage_t)100 - mainKnobValue;

    // Ensure there's a minimum blink period
    blinkPeriod = (blinkPeriod > 10) ? blinkPeriod : 10;

    return blinkPeriod;
}
#endif

/**
 * @rst
 * .. impl:: Light Controller's main function
 *    :id: SWIMPL_LC-006
 *    :implements: SWDD_LC-100
 *    :fulfills: REQ_44
 * @endrst
 *
 * @brief Controls the light state.
 *
 * Uses a state machine to determine the light state based on several inputs,
 * e.g., the system's power state.
 */
void lightController(void)
{
    static LightState currentLightState = LIGHT_OFF; /**< Current state of the light. */

    const PowerState currentPowerState = RteGetPowerState();
#ifdef CONFIG_BLINKING
    static uint32_t blinkCounter = 0;
    const percentage_t mainKnobValue = RteGetMainKnobValue();
    const uint32_t blinkPeriod = calculateBlinkPeriod(mainKnobValue);
#endif

    switch (currentLightState)
    {
    case LIGHT_OFF:
#ifdef CONFIG_BLINKING
        blinkCounter = 0;
#endif
        if (currentPowerState != POWER_STATE_OFF)
        {
            turnLightOn();
            currentLightState = LIGHT_ON;
        }
        break;

    default: // LIGHT_ON
        if (currentPowerState == POWER_STATE_OFF)
        {
            turnLightOff();
            currentLightState = LIGHT_OFF;
        }
#ifdef CONFIG_BLINKING
        else
        {
            blinkCounter++;
            if (blinkCounter >= blinkPeriod)
            {
                // Toggle the LED state
                if (blinkState == TRUE)
                {
                    turnLightOff();
                }
                else
                {
                    turnLightOn();
                }
                blinkCounter = 0;
            }
        }
#endif
#ifdef CONFIG_BRIGHTNESS_ADJUSTMENT_ENABLED
        else
        {
            turnLightOn();
        }
#endif
        break;
    }
}
