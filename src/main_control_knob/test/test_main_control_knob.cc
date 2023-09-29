/**
 * @file
 */

#include <gtest/gtest.h>
using namespace testing;

extern "C" {
#include "main_control_knob.h"
#include <windows.h>
}

#include "mockup_src_main_control_knob.h"

// Test cases for main_control_knob module
TEST(main_control_knob, arrow_up_increases_brightness)
{
     CREATE_MOCK(mymock);

    // Simulate Arrow Up key press.
    EXPECT_CALL(mymock, RteIsKeyPressed(VK_UP)).WillOnce(Return(TRUE));

    // Set initial knob value and color brightness
    EXPECT_CALL(mymock, RteGetMainKnobValue()).WillOnce(Return(50));
    
    // Call the function to handle knob input.
    knobControlInput();

    // Verify that knob value and color brightness increased by 5.
    EXPECT_CALL(mymock, RteSetMainKnobValue(55));
    RGBColor updatedColor = { .red = 100, .green = 105, .blue = 100 };
    EXPECT_CALL(mymock, RteSetLightValue(updatedColor));
}

 

TEST(main_control_knob, arrow_down_decreases_brightness)
{
     CREATE_MOCK(mymock);

    // Simulate Arrow Down key press.
    EXPECT_CALL(mymock, RteIsKeyPressed(VK_UP)).WillOnce(Return(FALSE)); // Arrow Up not pressed.
    EXPECT_CALL(mymock, RteIsKeyPressed(VK_DOWN)).WillOnce(Return(TRUE));     // Arrow Down pressed.

    // Set initial knob value and color brightness
    EXPECT_CALL(mymock, RteGetMainKnobValue()).WillOnce(Return(50));

    // Call the function to handle knob input.
    knobControlInput();

    // Verify that knob value and color brightness decreased by 5.
    EXPECT_CALL(mymock, RteSetMainKnobValue(45));
    RGBColor updatedColor = { .red = 100, .green = 95, .blue = 100 };
    EXPECT_CALL(mymock, RteSetLightValue(updatedColor));
}
