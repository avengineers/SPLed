/**
 * @file
 */
#include <gtest/gtest.h>
using namespace testing;

extern "C" {
#include "brightness_controller.h"
#include "rte.h"
}

// Auto-generated mockups for this component
#include "mockup_src_brightness_controller.h"


/*!
* @rst
*
* .. test:: brightness_controller.test_brightness_knob
*    :id: TS_BC-001
*    :tests: SWDD_BC-001, SWDD_BC-002
*
* @endrst
*/
TEST(brightness_controller, test_brightness_knob)
{
    CREATE_MOCK(mymock);

    InSequence seq; // Ensures that the expectations are called in the specified order

    // knob value 0 percent
    EXPECT_CALL(mymock, RteGetMainKnobValue()).WillOnce(Return(0));
    EXPECT_CALL(mymock, RteSetBrightnessValue(0));

    // knob value 100 percent
    EXPECT_CALL(mymock, RteGetMainKnobValue()).WillOnce(Return(100));
    EXPECT_CALL(mymock, RteSetBrightnessValue(255));

    // knob value 50 percent
    EXPECT_CALL(mymock, RteGetMainKnobValue()).WillOnce(Return(50));
    EXPECT_CALL(mymock, RteSetBrightnessValue(127)); // 50% of 255

    // knob value 0 percent
    EXPECT_CALL(mymock, RteGetMainKnobValue()).WillOnce(Return(0));
    EXPECT_CALL(mymock, RteSetBrightnessValue(0));

    // IUT
    for (int i = 0; i < 4; i++) {
        brightnessController();
    }
}
