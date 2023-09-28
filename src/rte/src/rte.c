#include "rte.h"
#include <windows.h>

static PowerState currentPowerState = POWER_STATE_OFF;
static boolean powerKeyPressed = FALSE;
static RGBColor lightValue = {
    .red = 0,
    .green = 0,
    .blue = 0
};

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
