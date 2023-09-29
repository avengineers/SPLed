#include "rte.h"
#include <windows.h>

static PowerState currentPowerState = POWER_STATE_OFF;
static boolean powerKeyPressed = FALSE;
static boolean arrowUpKeyPressed = FALSE;
static boolean arrowDownKeyPressed = FALSE;
static RGBColor lightValue = {
    .red = 0,
    .green = 0,
    .blue = 0
};
static percentage_t main_knob_value = 0;

void RteSetPowerState(PowerState state) {
    currentPowerState = state;
}

PowerState RteGetPowerState(void) {
    return currentPowerState;
}

void RteSetPowerKeyPressed(boolean value) {
    powerKeyPressed = value;
}

boolean RteGetPowerKeyPressed() {
    return powerKeyPressed;
}

void RteSetLightValue(RGBColor value) {
    lightValue = value;
}

void RteGetLightValue(RGBColor* value) {
    *value = lightValue;
}

boolean RteIsKeyPressed(int key) {
    return (GetAsyncKeyState(key) & 0x8000) != 0;
}


void RteSetMainKnobValue(percentage_t value) {
    if (value > 100) {
        main_knob_value = 100;
    } else {
        main_knob_value = value;
    }
}

percentage_t RteGetMainKnobValue(void) {
    return main_knob_value;
}
