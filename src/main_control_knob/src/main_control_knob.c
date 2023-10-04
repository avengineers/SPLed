/** @file main_control_knob.c
* @brief Implementation of the main control knob module.
*
* This file contains the implementation of the brightness of the light based on arrow up and down key presses
*/


#include "main_control_knob.h"
#include "rte.h"

/*!
* @rst
*
* .. impl:: Main Control Knob
*    :id: SWIMPL_MCK-001
*    :implements: SWDD_MCK-001
* @endrst
*/
// Function to handle knob control input.
void knobControlInput(void) {
    percentage_t currentValue = RteGetMainKnobValue();
    if (RteIsKeyPressed(CONTROL_KEY_UP)) {
        // Increase brightness by 5 with a maximum of 100.
        if (currentValue < (100 - KNOB_UPDATE_INCREMENT)) {
            currentValue += KNOB_UPDATE_INCREMENT;
        }
        else {
            currentValue = 100;
        }
    }
    else if (RteIsKeyPressed(CONTROL_KEY_DOWN)) {
        // Decrease brightness by 5 with a minimum of 0.
        if (currentValue > KNOB_UPDATE_INCREMENT) {
            currentValue -= KNOB_UPDATE_INCREMENT;
        }
        else {
            currentValue = 0;
        }
    }
    RteSetMainKnobValue(currentValue);
}