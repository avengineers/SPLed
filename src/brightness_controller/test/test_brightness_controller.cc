/**
 * @file
 */
#include <gtest/gtest.h>
using namespace testing;

extern "C" {
#include "brightness_controller.h"
#include "rte.h"

#ifdef CONFIG_BRIGHTNESS_ADJUSTMENT_PERIOD
    extern brightness_t periodicBrightnessAdjustment(BrightnessAdjustmentData* data);
#endif

}

// Auto-generated mockups for this component
#include "mockup_src_brightness_controller.h"

#ifdef CONFIG_BRIGHTNESS_ADJUSTMENT_PERIOD

TEST(brightness_controller, test_brightness_ramp)
{
    CREATE_MOCK(mymock);

    BrightnessAdjustmentData data = {
        .timeCounter = 0,
        .period = 8,
        .halfPeriod = 4,
        .maxBrightness = 200,
        .minBrightness = 100
    };

    struct {
        int calls;
        brightness_t expectedValue;
    } testCases[] = {
        {1, 100},
        {1, 125},
        {3, 200},
        {1, 175},
        {3, 100},
        {1, 125}
    };

    for (const auto& testCase : testCases) {
        brightness_t brightnessValue = 0;
        for (int i = 0; i < testCase.calls; ++i) {
            brightnessValue = periodicBrightnessAdjustment(&data);
        }
        EXPECT_EQ(brightnessValue, testCase.expectedValue);
    }
}

TEST(brightness_controller, test_brightness_ramp_high_values)
{
    CREATE_MOCK(mymock);

    BrightnessAdjustmentData data = {
        .timeCounter = 0,
        .period = 2000, // 20s -> 20000ms -> 2000 ticks
        .halfPeriod = 1000,
        .maxBrightness = 200,
        .minBrightness = 100
    };

    struct {
        int calls;
        brightness_t expectedValue;
    } testCases[] = {
        {1, 100},
        {1, 100},
        {100, 110},
        {100, 120},
    };

    for (const auto& testCase : testCases) {
        brightness_t brightnessValue = 0;
        for (int i = 0; i < testCase.calls; ++i) {
            brightnessValue = periodicBrightnessAdjustment(&data);
        }
        EXPECT_EQ(brightnessValue, testCase.expectedValue);
    }
}

#else
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


#endif
