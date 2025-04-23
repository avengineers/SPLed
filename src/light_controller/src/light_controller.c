/**
 * @file light_controller.c
 * @brief Module to control light based on power state.
 */

#include "light_controller.h"
#include "rte.h"
#include "autoconf.h"

 /**
  * @enum LightColor
  * @brief Represents the possible light colors.
  */
typedef enum {
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
 *    :implements: SWDD_LC-001
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
#if CONFIG_BLINKING_RATE_AUTO_ADJUSTABLE 
static int blinkCounter = 0;
static boolean blinkState = FALSE;
#endif
const RGBColor OFF_COLOR = { .red = 0, .green = 0, .blue = 0 };

const LightColor light_colors[] = {
#if CONFIG_COLOR_GREEN
    COLOR_VAL_GREEN
#elif CONFIG_COLOR_BLUE
    COLOR_VAL_BLUE
#elif CONFIG_COLOR_RED
    COLOR_VAL_RED
#elif CONFIG_COLOR_PURPLE
    COLOR_VAL_PURPLE
#else
    COLOR_VAL_OFF
#endif
#if CONFIG_COLOR_1_IS_ENABLED
    ,
#if CONFIG_COLOR_1_GREEN
    COLOR_VAL_GREEN
#elif CONFIG_COLOR_1_BLUE
    COLOR_VAL_BLUE
#elif CONFIG_COLOR_1_RED
    COLOR_VAL_RED
#elif CONFIG_COLOR_1_PURPLE
    COLOR_VAL_PURPLE
#else
    COLOR_VAL_OFF
#endif
#endif // CONFIG_COLOR_1_IS_ENABLED
};
const int light_colors_count = sizeof(light_colors) / sizeof(light_colors[0]);
// Used to iterate through the light colors
int light_colors_index = 0;

static brightness_t getBrightnessValue() {
#ifdef CONFIG_BRIGHTNESS_ADJUSTMENT_IS_ENABLED
    /**
     * @rst
     * .. impl:: Variable brightness
     *    :id: SWIMPL_LC-005
     *    :implements: SWDD_LC-005
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
static RGBColor getRGBColorWithBrightness(LightColor colorEnum, brightness_t brightness) {
    RGBColor color = OFF_COLOR;
    switch (colorEnum) {
    case COLOR_VAL_GREEN:
        color.green = brightness;
        break;
    case COLOR_VAL_BLUE:
        color.blue = brightness;
        break;
    case COLOR_VAL_RED:
        color.red = brightness;
        break;
    case COLOR_VAL_PURPLE:
        color.red = brightness / 2; // Assuming purple is half red, full blue
        color.blue = brightness;
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
 *    :implements: SWDD_LC-003
 * @endrst
 */
static void turnLightOff(void) {
#if CONFIG_BLINKING_RATE_AUTO_ADJUSTABLE 
    blinkState = FALSE;
#endif
    RteSetLightValue(OFF_COLOR);
}

/**
 * @rst
 * .. impl:: Turn light on
 *    :id: SWIMPL_LC-003
 *    :implements: SWDD_LC-003
 * @endrst
 */
static void turnLightOn(void) {
#ifdef CONFIG_BRIGHTNESS_ADJUSTMENT_PERIOD
    // Check if the brightness adjustment counter is zero to switch colors
    unsigned int counter = 0;
    RteGetBrightnessAdjustmentCounter(&counter);
    if (counter == 0) {
        light_colors_index = (light_colors_index + 1) % light_colors_count;
    }
#else
    light_colors_index = 0;
#endif

    // Get the current color enum and brightness
    LightColor currentColorEnum = light_colors[light_colors_index];
    brightness_t currentBrightness = getBrightnessValue();

    // Convert to RGBColor and set the light value
    RGBColor color = getRGBColorWithBrightness(currentColorEnum, currentBrightness);

#if CONFIG_BLINKING_RATE_AUTO_ADJUSTABLE
    blinkState = TRUE;
#endif
    RteSetLightValue(color);
}

#if CONFIG_BLINKING_RATE_AUTO_ADJUSTABLE 
/**
 * @rst
 *
 * .. impl:: Calculate blink period
 *    :id: SWIMPL_LC-004
 *    :implements: SWDD_LC-002
 * @endrst
 */
SPLE_TESTABLE_STATIC unsigned int calculateBlinkPeriod(percentage_t mainKnobValue) {
    // Calculate blink period based on main knob value
    unsigned int blinkPeriod = 100 - (mainKnobValue); // Adjust this formula as needed

    // Ensure there's a minimum blink period
    blinkPeriod = (blinkPeriod > 10) ? blinkPeriod : 10; // Adjust the minimum period as needed

    return blinkPeriod;
}
#endif

/**
 * @rst
 * .. impl:: Light Controller's main function
 *    :id: SWIMPL_LC-006
 *    :implements: SWDD_LC-001
 * @endrst
 *
 * @brief Controls the light state.
 *
 * Uses a state machine to determine the light state based on several inputs,
 * e.g., the system's power state.
 */
void lightController(void) {

    PowerState powerState = RteGetPowerState();
#if CONFIG_BLINKING_RATE_AUTO_ADJUSTABLE 
    percentage_t mainKnobValue = RteGetMainKnobValue();
    unsigned int blinkPeriod = calculateBlinkPeriod(mainKnobValue);
#endif

#if LOGGING_ENABLED
    RteLoggerPrintToConsole(LOG_LEVEL_DEBUG, "Light controller: power state = %d, main knob value = %d, blink period = %d", powerState, mainKnobValue, blinkPeriod);
#endif

    switch (currentLightState) {
    case LIGHT_OFF:
#if CONFIG_BLINKING_RATE_AUTO_ADJUSTABLE 
        blinkCounter = 0;
#endif
        if (powerState != POWER_STATE_OFF) {
            turnLightOn();
            currentLightState = LIGHT_ON;
        }
        break;

    default: // LIGHT_ON
        if (powerState == POWER_STATE_OFF) {
            turnLightOff();
            currentLightState = LIGHT_OFF;
        }
#if CONFIG_BLINKING_RATE_AUTO_ADJUSTABLE 
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
#endif
#ifdef CONFIG_BRIGHTNESS_ADJUSTMENT_IS_ENABLED
        else {
            turnLightOn();
        }
#endif
        break;
    }
}
