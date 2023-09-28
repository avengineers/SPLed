#include <stdio.h>

#include "spled.h"

#include "keyboard_interface.h"
#include "power_signal_processing.h"
#include "light_controller.h"
#include "console_interface.h"

void spled(void) {
    keyboardInterface();
    powerSignalProcessing();
    lightController();
    consoleInterface();
}
