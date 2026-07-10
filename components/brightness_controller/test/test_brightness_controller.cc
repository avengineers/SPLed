/**
 * @file
 */
#include <gtest/gtest.h>
using namespace testing;

extern "C"
{
#include "autoconf.h"
#include "brightness_controller.h"
#include "rte.h"

#ifdef CONFIG_BRIGHTNESS_ADJUSTMENT_AUTOMATIC
    extern brightness_t periodicBrightnessAdjustment(BrightnessAdjustmentData *data);
#endif
}

// Auto-generated mockups for this component
#include "mockup_components_brightness_controller.h"

#ifdef CONFIG_BRIGHTNESS_ADJUSTMENT_AUTOMATIC

/**
 * @rst
 * .. test:: brightness_controller.test_brightness_ramp
 *    :id: TS_BC-002
 *   :tests: SWDD_BC-100, SWDD_BC-102, SWDD_BC-203
 * @endrst
 */
TEST(brightness_controller, test_brightness_ramp)
{
    /* Arrange */
    CREATE_MOCK(mymock);

    BrightnessAdjustmentData data = {
        .ticksCounter = 0,
        .period = 8,
        .halfPeriod = 4,
        .maxBrightness = 200,
        .minBrightness = 100};

    struct
    {
        int calls;
        brightness_t expectedValue;
    } testCases[] = {
        {1, 100},
        {1, 125},
        {3, 200},
        {1, 175},
        {3, 100},
        {1, 125}};

    for (const auto &testCase : testCases)
    {
        brightness_t brightnessValue = 0;
        for (int i = 0; i < testCase.calls; ++i)
        {
            /* Act */
            brightnessValue = periodicBrightnessAdjustment(&data);
        }
        /* Assert */
        EXPECT_EQ(brightnessValue, testCase.expectedValue);
    }
}

/**
 * @rst
 * .. test:: brightness_controller.test_brightness_controller_auto
 *    :id: TS_BC-003
 *    :tests: SWDD_BC-200, SWDD_BC-202, SWDD_BC-203
 * @endrst
 */
TEST(brightness_controller, test_brightness_controller_auto)
{
    const uint16_t halfPeriodTicks = CONFIG_BRIGHTNESS_ADJUSTMENT_PERIOD * 1000 / 10 / 2;
    const brightness_t maxBrightness = 199;
    const brightness_t minBrightness = 50;
    const uint16_t ticksWithConstantBrightness = halfPeriodTicks / (maxBrightness - minBrightness);

    // Arrange
    CREATE_MOCK(mymock);

    EXPECT_CALL(mymock, RteSetBrightnessAdjustmentCounter(_)).Times(halfPeriodTicks);
    EXPECT_CALL(mymock, RteSetBrightnessValue(_)).Times(halfPeriodTicks - ticksWithConstantBrightness);
    // The last ticks before half the period we should reach the max brightness
    EXPECT_CALL(mymock, RteSetBrightnessValue(maxBrightness)).Times(ticksWithConstantBrightness);

    // Act
    for (uint16_t i = 0; i < halfPeriodTicks; ++i)
    {
        brightnessController();
    }

    // Assert
    // The expectations are already checked by the mock framework
}

#else /* CONFIG_BRIGHTNESS_ADJUSTMENT_AUTOMATIC */

/**
 * @rst
 * .. test:: brightness_controller.test_brightness_knob
 *    :id: TS_BC-001
 *    :tests: SWDD_BC-100, SWDD_BC-101, SWDD_BC-200, SWDD_BC-201, SWDD_BC-202
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
    for (int i = 0; i < 4; i++)
    {
        brightnessController();
    }
}

#endif /* CONFIG_BRIGHTNESS_ADJUSTMENT_AUTOMATIC */
