/**
 * @file
 */
#include <gtest/gtest.h>
using namespace testing;

extern "C" {
#include "light_controller.h"
#include "rte.h"
#include "autoconf.h"
    extern unsigned int calculateBlinkPeriod(percentage_t mainKnobValue);
}

#include "mockup_src_light_controller.h" // Assuming you have mock sources for the light controller.

bool areRGBColorsEqual(const RGBColor* color1, const RGBColor* color2) {
    return color1->red == color2->red &&
        color1->green == color2->green &&
        color1->blue == color2->blue;
}

MATCHER_P(RGBColorEq, expected, "") {
    return areRGBColorsEqual(&arg, &expected);
}

#if CONFIG_BRIGHTNESS_ADJUSTMENT
#define GREEN_LED_BRIGHTNESS 200
#else 
#define GREEN_LED_BRIGHTNESS 128
#endif

const RGBColor onColor = { .red = 0, .green = GREEN_LED_BRIGHTNESS, .blue = 55 };
const RGBColor offColor = { .red = 0, .green = 0, .blue = 0 };

// Override the cout operator for RGBColor so that it can be printed in the test output
std::ostream& operator<<(std::ostream& os, const RGBColor& color)
{
    os << "RGBColor(" << color.red << ", " << color.green << ", " << color.blue << ")";
    return os;
}

/*!
* @rst
*
* .. test:: light_controller.test_light_on_and_off
*    :id: TS_LC-001
*    :results: [[tr_link('title', 'case')]]
*    :tests: SWDD_LC-001, SWDD_LC-003, SWDD_LC-004
*
* @endrst
*/
TEST(light_controller, test_light_on_and_off)
{
    CREATE_MOCK(mymock);

    // Initial state: Power is OFF, so the light should be OFF.
    EXPECT_CALL(mymock, RteGetPowerState()).WillOnce(Return(POWER_STATE_OFF));
    EXPECT_CALL(mymock, RteSetLightValue(_)).Times(0); // Expect that the light value doesn't change.
    lightController();

    // Power turns ON, so the light should turn ON.
    EXPECT_CALL(mymock, RteGetPowerState()).WillRepeatedly(Return(POWER_STATE_ON));
#if CONFIG_BRIGHTNESS_ADJUSTMENT
    EXPECT_CALL(mymock, RteGetBrightnessValue()).WillRepeatedly(Return(GREEN_LED_BRIGHTNESS));
    EXPECT_CALL(mymock, RteSetLightValue(RGBColorEq(onColor))).Times(11);
#else
    // Expect that the light value changes to ON color (only once, no redundant updates).
    EXPECT_CALL(mymock, RteSetLightValue(RGBColorEq(onColor))).Times(1);
#endif
    for (int i = 0; i < 10; i++) {
        lightController();
    }

    // Power turns OFF, so the light should turn OFF.
    EXPECT_CALL(mymock, RteGetPowerState()).WillRepeatedly(Return(POWER_STATE_OFF));
    // Expect that the light value changes to OFF color (only once, no redundant updates).
    EXPECT_CALL(mymock, RteSetLightValue(RGBColorEq(offColor))).Times(1);
    for (int i = 0; i < 10; i++) {
        lightController();
    }
}

#if CONFIG_BRIGHTNESS_ADJUSTMENT
/*!
* @rst
*
* .. test:: light_controller.test_light_on_very_bright
*    :id: TS_LC-005
*    :results: [[tr_link('title', 'case')]]
*    :tests: SWDD_LC-005
*
* @endrst
*/
TEST(light_controller, test_light_on_very_bright)
{
    CREATE_MOCK(mymock);

    // Power ON, so the light should turn ON.
    EXPECT_CALL(mymock, RteGetPowerState()).WillRepeatedly(Return(POWER_STATE_ON));
    for (int i = 0; i < 3; i++) {
        EXPECT_CALL(mymock, RteGetBrightnessValue()).WillRepeatedly(Return(GREEN_LED_BRIGHTNESS));
        EXPECT_CALL(mymock, RteSetLightValue(RGBColorEq(onColor))).Times(1);
        lightController();
    }
}
#endif

#if CONFIG_BLINKING_RATE_AUTO_ADJUSTABLE 
/*!
* @rst
*
* .. test:: light_controller.test_light_blinking
*    :id: TS_LC-002
*    :results: [[tr_link('title', 'case')]]
*    :tests: SWDD_LC-002
*
* @endrst
*/
TEST(light_controller, test_light_blinking)
{
    CREATE_MOCK(mymock);

    // Set the initial power state to ON so the light can blink
    EXPECT_CALL(mymock, RteGetPowerState()).WillRepeatedly(Return(POWER_STATE_ON));

    // Assume main knob value is 50, so blinkPeriod will be 50.
    EXPECT_CALL(mymock, RteGetMainKnobValue()).WillRepeatedly(Return(50));

    // Expect that the light value changes to ON color (only once, no redundant updates).
    EXPECT_CALL(mymock, RteSetLightValue(RGBColorEq(onColor))).Times(1);
    lightController();

    // Call lightController repeatedly to simulate time passing and check the light state
    for (int i = 0; i < 49; i++) {
        EXPECT_CALL(mymock, RteSetLightValue(_)).Times(0);
        lightController();
    }

    // Expect that the light value changes to OFF color (only once, no redundant updates).
    EXPECT_CALL(mymock, RteSetLightValue(RGBColorEq(offColor))).Times(1);
    lightController();

    // Call lightController repeatedly to simulate time passing and check the light state
    for (int i = 0; i < 49; i++) {
        EXPECT_CALL(mymock, RteSetLightValue(_)).Times(0);
        lightController();
    }

    // Expect that the light value changes to ON color (only once, no redundant updates).
    EXPECT_CALL(mymock, RteSetLightValue(RGBColorEq(onColor))).Times(1);
    lightController();
}

// Define a test fixture class
class BlinkPeriodTest : public ::testing::TestWithParam<struct TestParam> {
};

// Define a struct to hold the parameters for each test case
struct TestParam {
    percentage_t mainKnobValue;
    unsigned int expectedBlinkPeriod;
};

/*!
* @rst
*
* .. test:: BlinkPeriodTest.CalculatesCorrectBlinkPeriod
*    :id: TS_LC-003
*    :results: [[tr_link('title', 'case')]]
*    :tests: SWDD_LC-002
*
* @endrst
*/
TEST_P(BlinkPeriodTest, CalculatesCorrectBlinkPeriod)
{
    // Get the parameters for this test case
    TestParam param = GetParam();

    unsigned int blinkPeriod = calculateBlinkPeriod(param.mainKnobValue);
    EXPECT_EQ(blinkPeriod, param.expectedBlinkPeriod);
}

// Instantiate the test suite with a set of parameters
INSTANTIATE_TEST_SUITE_P(
    BlinkPeriodTests,
    BlinkPeriodTest,
    ::testing::Values(
        TestParam{ 0, 100 },
        TestParam{ 50, 50 },
        TestParam{ 100, 10 }
    )
);
#endif // CONFIG_BLINKING_RATE_AUTO_ADJUSTABLE
