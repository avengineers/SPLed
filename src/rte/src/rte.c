#include "rte.h"
#include <windows.h>
#include <stdio.h>
#include "autoconf.h"

static PowerState currentPowerState = POWER_STATE_OFF;
static boolean powerKeyPressed = FALSE;
static boolean arrowUpKeyPressed = FALSE;
static boolean arrowDownKeyPressed = FALSE;
static RGBColor lightValue = {
    .red = 0,
    .green = 0,
    .blue = 0
};
static percentage_t main_knob_value = 50;
static unsigned int brightnessValue = 0;
int blinkSpeed = 50;

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
    }
    else {
        main_knob_value = value;
    }
}

percentage_t RteGetMainKnobValue(void) {
    return main_knob_value;
}


void RteSetBrightnessValue(unsigned int value) {
    brightnessValue = value;
}

unsigned int RteGetBrightnessValue(void) {
    return brightnessValue;
}



#if LOGGING_ENABLED
static const char* LogLevelToString(LogLevel level) {
    switch (level) {
    case LOG_LEVEL_ERROR:   return "ERROR";
    case LOG_LEVEL_WARNING: return "WARNING";
    case LOG_LEVEL_INFO:    return "INFO";
    case LOG_LEVEL_DEBUG:   return "DEBUG";
    default:          return "UNKNOWN";
    }
}

void RteLoggerPrintToConsole(LogLevel level, const char* message, ...) {
    va_list args;
    va_start(args, message);

    // Print log level
    printf("[%s] ", LogLevelToString(level));
    // Print message
    vprintf(message, args);
    // Print a new line
    printf("\n");

    va_end(args);
}
#endif
