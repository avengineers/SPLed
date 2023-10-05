#include "autoconf.h"
#include "spled.h"

#include "keyboard_interface.h"
#include "power_signal_processing.h"
#include "light_controller.h"
#include "console_interface.h"
#include "main_control_knob.h"
#if defined(CONFIG_BRIGHTNESS_ADJUSTMENT) && CONFIG_BRIGHTNESS_ADJUSTMENT == 1
#include "brightness_controller.h"
#endif
#include <stdio.h>
#include <unistd.h>

void spled(void) {
    keyboardInterface();
    powerSignalProcessing();
    mainControlKnob();
#if defined(CONFIG_BRIGHTNESS_ADJUSTMENT) && CONFIG_BRIGHTNESS_ADJUSTMENT == 1
    brightnessController();
#endif
    lightController();
    consoleInterface();
    // simple main loop of 10 milliseconds
    usleep(10000);
}
