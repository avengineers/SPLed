/**
 * @file brightness_controller.c
 * @brief Module to control the brightness based on main knob value and light value.
*/

#include "brightness_controller.h"
#include "rte.h"

/*!
* @rst
*
* .. impl:: Calculate brightness of the light
*    :id: SWIMPL_BC-001
*    :implements: SWDD_BC-001, SWDD_BC-002
* @endrst
*/
void brightnessController(void) {
    percentage_t mainKnobValue = RteGetMainKnobValue();
    unsigned int brightnessValue = 0;

    if (mainKnobValue == 0) {
        brightnessValue = 0;
    }
    else {
        brightnessValue = (mainKnobValue * 255) / 100;
    }
    RteSetBrightnessValue(brightnessValue);
}
