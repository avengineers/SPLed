/**
 * @file
 */
#include <gtest/gtest.h>
using namespace testing;

extern "C" {
#include "brightness_controller.h"
#include "rte.h"
}

#include "mockup_src_brightness_controller.h" // Assuming you have mock sources for the brightness controller.


/*!
* @rst
*
* .. test:: brightness_controller.test_increase_brightness
*    :id: TS_BC-001
*    :results: [[tr_link('title', 'case')]]
*    :specified: SWDD_BC-001
*
* @endrst
*/
TEST(brightness_controller, test_increase_brightness)
{
    CREATE_MOCK(mymock);

    // Set initial knob value
    EXPECT_CALL(mymock, RteGetMainKnobValue()).WillOnce(Return(50));

    // Verify that brightness value was increased.
    EXPECT_CALL(mymock, RteSetBrightnessValue(127)); // 50% of 255

    // Call the brightnessController function.
    brightnessController();
}


/*!
* @rst
*
* .. test:: brightness_controller.test_decrease_brightness
*    :id: TS_BC-002
*    :results: [[tr_link('title', 'case')]]
*    :specified: SWDD_BC-002
*
* @endrst
*/
TEST(brightness_controller, test_decrease_brightness)
{
    CREATE_MOCK(mymock);

    // Set initial main knob value and brightness value
    EXPECT_CALL(mymock, RteGetMainKnobValue()).WillOnce(Return(70));

    // Verify that brightness value was decreased.
    EXPECT_CALL(mymock, RteSetBrightnessValue(178)); // 70% of 255

    // Call the brightnessController function.
    brightnessController();
}


/*!
* @rst
*
* .. test:: brightness_controller.test_brightness_does_not_decrease_below_zero
*    :id: TS_BC-003
*    :results: [[tr_link('title', 'case')]]
*    :specified: SWDD_BC-002
*
* @endrst
*/
TEST(brightness_controller, test_brightness_does_not_decrease_below_zero) {
    CREATE_MOCK(mymock);

    // Set initial main knob value and brightness value
    EXPECT_CALL(mymock, RteGetMainKnobValue()).WillOnce(Return(0));

    // Verify that brightness value does not go below 0.
    EXPECT_CALL(mymock, RteSetBrightnessValue(0));

    // Call the brightnessController function.
    brightnessController();
}




/*!
* @rst
*
* .. test:: brightness_controller.test_brightness_does_not_increase_above_255
*    :id: TS_BC-004
*    :results: [[tr_link('title', 'case')]]
*    :specified: SWDD_BC-001
*
* @endrst
*/
TEST(brightness_controller, test_brightness_does_not_increase_above_255) {
    CREATE_MOCK(mymock);

    // Set initial main knob value and brightness value
    EXPECT_CALL(mymock, RteGetMainKnobValue()).WillOnce(Return(100));

    // Verify that brightness value does not go above 255.
    EXPECT_CALL(mymock, RteSetBrightnessValue(255));

    // Call the brightnessController function.
    brightnessController();
}
