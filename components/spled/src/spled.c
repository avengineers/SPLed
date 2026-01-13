#include "autoconf.h"
#include "spled.h"

#include "power_button.h"
#include "power_signal_processing.h"
#include "light_controller.h"
#include "console_interface.h"
#include "main_control_knob.h"
#ifdef CONFIG_AUTO_OFF
#include "auto_off.h"
#endif
#ifdef CONFIG_BRIGHTNESS_ADJUSTMENT_ENABLED
#include "brightness_controller.h"
#endif

void spled(void)
{
    powerButton();
    powerSignalProcessing();
    mainControlKnob();
#ifdef CONFIG_AUTO_OFF
    autoOff();
#endif
#ifdef CONFIG_BRIGHTNESS_ADJUSTMENT_ENABLED
    brightnessController();
#endif
    lightController();
    consoleInterface();
}
