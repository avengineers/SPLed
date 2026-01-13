#include "rte.h"
#include "keyboard_interface.h"

static PowerState rteCurrentPowerState = POWER_STATE_OFF;
#ifdef CONFIG_AUTO_OFF
static bool_t autoOffState = TRUE;
#endif
static bool_t rtePowerKeyPressedEvent = FALSE;
static RGBColor rteLightValue = {
    .rgbRedValue = 0,
    .rgbGreenValue = 0,
    .rgbBlueValue = 0,
};
static percentage_t rteMainKnobValue = 50;
static brightness_t rteBrightnessValue = 0;

#ifdef CONFIG_BRIGHTNESS_ADJUSTMENT_PERIOD
static uint32_t brightnessAdjustmentCounter = 0;
#endif

// Flight Controller RTE variables
static bool_t rteOffCourse = FALSE;
static bool_t rteAbortCommanded = FALSE;
static bool_t rteValidAbortCommand = FALSE;
static bool_t rteSelfDestructState = FALSE;

void RteSetPowerState(PowerState currentPowerState)
{
    rteCurrentPowerState = currentPowerState;
}

PowerState RteGetPowerState(void)
{
    return rteCurrentPowerState;
}

void RteSetPowerKeyPressedEvent(bool_t powerKeyPressedEvent)
{
    rtePowerKeyPressedEvent = powerKeyPressedEvent;
}

bool_t RteGetPowerKeyPressedEvent(void)
{
    return rtePowerKeyPressedEvent;
}

void RteSetLightValue(const RGBColor lightValue)
{
    rteLightValue = lightValue;
}

void RteGetLightValue(RGBColor *const lightValue)
{
    *lightValue = rteLightValue;
}

bool_t RteIsKeyPressed(int32_t keyCode)
{
    return KeyboardInterfaceIsKeyPressed(keyCode);
}

void RteSetMainKnobValue(percentage_t mainKnobValue)
{
    rteMainKnobValue = mainKnobValue;
}

percentage_t RteGetMainKnobValue(void)
{
    return rteMainKnobValue;
}

void RteSetBrightnessValue(brightness_t brightnessValue)
{
    rteBrightnessValue = brightnessValue;
}

brightness_t RteGetBrightnessValue(void)
{
    return rteBrightnessValue;
}

#ifdef CONFIG_BRIGHTNESS_ADJUSTMENT_PERIOD
void RteSetBrightnessAdjustmentCounter(uint32_t counter)
{
    brightnessAdjustmentCounter = counter;
}

void RteGetBrightnessAdjustmentCounter(uint32_t *const counter)
{
    *counter = brightnessAdjustmentCounter;
}
#endif // CONFIG_BRIGHTNESS_ADJUSTMENT_PERIOD

void RteSetOffCourse(bool_t offCourse)
{
    rteOffCourse = offCourse;
}

void RteGetOffCourse(bool_t *const offCourse)
{
    if (offCourse != NULL)
    {
        *offCourse = rteOffCourse;
    }
}

void RteSetAbortCommanded(bool_t commanded)
{
    rteAbortCommanded = commanded;
}

bool_t RteGetAbortCommanded(void)
{
    return rteAbortCommanded;
}

void RteSetValidAbortCommand(bool_t valid)
{
    rteValidAbortCommand = valid;
}

bool_t RteGetValidAbortCommand(void)
{
    return rteValidAbortCommand;
}

void RteSetSelfDestructState(bool_t selfDestructState)
{
    rteSelfDestructState = selfDestructState;
}

bool_t RteGetSelfDestructState(void)
{
    return rteSelfDestructState;
}

#ifdef CONFIG_AUTO_OFF
bool_t RteGetAutoOffState(void)
{
    return autoOffState;
}
#endif // CONFIG_AUTO_OFF

#ifdef CONFIG_AUTO_OFF
void RteSetAutoOffState(bool_t state)
{
    autoOffState = state;
}
#endif // CONFIG_AUTO_OFF
