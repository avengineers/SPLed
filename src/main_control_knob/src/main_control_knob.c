/** @file main_control_knob.c
* @brief Implementation of the main control knob module.
*
* This file contains the implementation of the brightness of the light based on arrow up and down key presses
*/


#include "main_control_knob.h"
#include <windows.h>
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
    if (RteIsKeyPressed(VK_UP)) {
        // Arrow Up key is pressed.
        percentage_t currentValue = RteGetMainKnobValue();
        if (currentValue < 100) {
            // Increment brightness by 5 with a maximum of 100.
            currentValue += 5;
            RteSetMainKnobValue(currentValue);

            // Adjust the color brightness accordingly (for example, increasing green component).
            RGBColor currentColor;
            RteGetLightValue(&currentColor);
            if (currentColor.green < 255) {
                currentColor.green += 5; // Increase green component.
            }
            RteSetLightValue(currentColor);
        }
    } else if (RteIsKeyPressed(VK_DOWN)) {
        // Arrow Down key is pressed.
        percentage_t currentValue = RteGetMainKnobValue();
        if (currentValue > 0) {
            // Decrease brightness by 5 with a minimum of 0.
            currentValue -= 5;
            RteSetMainKnobValue(currentValue);
            // Adjust the color brightness accordingly (for example, decreasing green component).
            RGBColor currentColor;
            RteGetLightValue(&currentColor);
            if (currentColor.green > 0) {
                currentColor.green -= 5; // Decrease green component.
            }
            RteSetLightValue(currentColor);
        }
    }
}