#include "rte.h"

// Internal variable to hold the power state
static PowerState currentPowerState = POWER_STATE_OFF;
static boolean powerKeyPressed = FALSE;
static int lightValue = 0;

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

void RteSetLightValue(int value) {
    lightValue = value;
}

int RteGetLightValue(void) {
    return lightValue;
}