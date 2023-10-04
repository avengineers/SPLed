#include "spled.h"

#include "keyboard_interface.h"
#include "power_signal_processing.h"
#include "light_controller.h"
#include "console_interface.h"
#include "main_control_knob.h"

#include <stdio.h>
#include <unistd.h>

void spled(void) {
    keyboardInterface();
    powerSignalProcessing();
    lightController();
    knobControlInput();
    consoleInterface();
    // simple main loop of 10 milliseconds
    usleep(10000);
}
