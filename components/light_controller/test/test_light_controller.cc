/**
 * @file
 */
#include <gtest/gtest.h>
using namespace testing;

extern "C"
{
#include "light_controller.h"
#include "rte.h"
#include "autoconf.h"
    extern unsigned int calculateBlinkPeriod(percentage_t mainKnobValue);
}

#include "mockup_components_light_controller.h" // Assuming you have mock sources for the light controller.

bool areRGBColorsEqual(const RGBColor *color1, const RGBColor *color2)
{
    return color1->rgbRedValue == color2->rgbRedValue &&
           color1->rgbGreenValue == color2->rgbGreenValue &&
           color1->rgbBlueValue == color2->rgbBlueValue;
}

MATCHER_P(RGBColorEq, expected, "")
{
    return areRGBColorsEqual(&arg, &expected);
}

#ifdef CONFIG_BRIGHTNESS_ADJUSTMENT_ENABLED
#define LED_BRIGHTNESS 200
#else
#define LED_BRIGHTNESS 128
#endif

#if CONFIG_COLOR_BLUE
const RGBColor onColor = {.rgbRedValue = 0, .rgbGreenValue = 0, .rgbBlueValue = LED_BRIGHTNESS};
#elif CONFIG_COLOR_GREEN
const RGBColor onColor = {.rgbRedValue = 0, .rgbGreenValue = LED_BRIGHTNESS, .rgbBlueValue = 0};
#elif CONFIG_COLOR_RED
const RGBColor onColor = {.rgbRedValue = LED_BRIGHTNESS, .rgbGreenValue = 0, .rgbBlueValue = 0};
#elif CONFIG_COLOR_PURPLE
const RGBColor onColor = {.rgbRedValue = LED_BRIGHTNESS / 2, .rgbGreenValue = 0, .rgbBlueValue = LED_BRIGHTNESS};
#endif /* CONFIG_COLOR_BLUE */

#if CONFIG_COLOR_1_BLUE
const RGBColor onColor1 = {.rgbRedValue = 0, .rgbGreenValue = 0, .rgbBlueValue = LED_BRIGHTNESS};
#elif CONFIG_COLOR_1_GREEN
const RGBColor onColor1 = {.rgbRedValue = 0, .rgbGreenValue = LED_BRIGHTNESS, .rgbBlueValue = 0};
#elif CONFIG_COLOR_1_RED
const RGBColor onColor1 = {.rgbRedValue = LED_BRIGHTNESS, .rgbGreenValue = 0, .rgbBlueValue = 0};
#elif CONFIG_COLOR_1_PURPLE
const RGBColor onColor1 = {.rgbRedValue = LED_BRIGHTNESS / 2, .rgbGreenValue = 0, .rgbBlueValue = LED_BRIGHTNESS};
#endif /* CONFIG_COLOR_1_BLUE */

const RGBColor offColor = {.rgbRedValue = 0, .rgbGreenValue = 0, .rgbBlueValue = 0};

// Override the cout operator for RGBColor so that it can be printed in the test output
std::ostream &operator<<(std::ostream &os, const RGBColor &color)
{
    os << "RGBColor(" << color.rgbRedValue << ", " << color.rgbGreenValue << ", " << color.rgbBlueValue << ")";
    return os;
}

/**
 * @rst
 *
 * .. test:: light_controller.test_light_stays_off
 *    :id: TS_LC-006
 *    :tests: SWDD_LC-100, SWDD_LC-300
 *
 * @endrst
 */
TEST(light_controller, test_light_stays_off)
{
    CREATE_MOCK(mymock);

    // Initial state: Power is OFF, so the light should be OFF.
    EXPECT_CALL(mymock, RteGetPowerState()).WillRepeatedly(Return(POWER_STATE_OFF));
    EXPECT_CALL(mymock, RteSetLightValue(_)).Times(0); // Expect that the light value doesn't change.

    for (int i = 0; i < 10; i++)
    {
        lightController();
    }
}

#ifdef CONFIG_BRIGHTNESS_ADJUSTMENT_AUTOMATIC
/**
 * @rst
 * .. test:: light_controller.test_brightness_adjustment_automatic
 *    :id: TS_LC-005
 *    :tests: SWDD_LC-100, SWDD_LC-200, SWDD_LC-201, SWDD_LC-202
 * @endrst
 */
TEST(light_controller, test_brightness_adjustment_automatic)
{
    CREATE_MOCK(mymock);

    // Set the initial power state to ON so the light can be adjusted
    EXPECT_CALL(mymock, RteGetPowerState()).WillRepeatedly(Return(POWER_STATE_ON));

    // Set the brightness adjustment counter to 1 to get the first color
    EXPECT_CALL(mymock, RteGetBrightnessValue()).WillRepeatedly(Return(LED_BRIGHTNESS));
    EXPECT_CALL(mymock, RteGetBrightnessAdjustmentCounter(_)).WillOnce(SetArgPointee<0>(1));
    EXPECT_CALL(mymock, RteSetLightValue(RGBColorEq(onColor))).Times(1);
    lightController();

    // Set the brightness adjustment counter to 0 to get the next color
    EXPECT_CALL(mymock, RteGetBrightnessAdjustmentCounter(_)).WillOnce(SetArgPointee<0>(0));
    EXPECT_CALL(mymock, RteSetLightValue(RGBColorEq(onColor1))).Times(1);
    lightController();
}

#else /* CONFIG_BRIGHTNESS_ADJUSTMENT_AUTOMATIC */

/**
 * @rst
 *
 * .. test:: light_controller.test_light_on_and_off
 *    :id: TS_LC-001
 *    :tests: SWDD_LC-100, SWDD_LC-102, SWDD_LC-300
 *
 * @endrst
 */
TEST(light_controller, test_light_on_and_off)
{
    CREATE_MOCK(mymock);

    EXPECT_CALL(mymock, RteGetPowerState()).WillOnce(Return(POWER_STATE_OFF));
    lightController();

    // Power turns ON, so the light should turn ON.
    EXPECT_CALL(mymock, RteGetPowerState()).WillRepeatedly(Return(POWER_STATE_ON));
#ifdef CONFIG_BRIGHTNESS_ADJUSTMENT_ENABLED
    EXPECT_CALL(mymock, RteGetBrightnessValue()).WillRepeatedly(Return(LED_BRIGHTNESS));
    EXPECT_CALL(mymock, RteSetLightValue(RGBColorEq(onColor))).Times(10);
#else
    // Expect that the light value changes to ON color (only once, no redundant updates).
    EXPECT_CALL(mymock, RteSetLightValue(RGBColorEq(onColor))).Times(1);
#endif
    for (int i = 0; i < 10; i++)
    {
        lightController();
    }

    // Power turns OFF, so the light should turn OFF.
    EXPECT_CALL(mymock, RteGetPowerState()).WillRepeatedly(Return(POWER_STATE_OFF));
    // Expect that the light value changes to OFF color (only once, no redundant updates).
    EXPECT_CALL(mymock, RteSetLightValue(RGBColorEq(offColor))).Times(1);
    for (int i = 0; i < 10; i++)
    {
        lightController();
    }
}
#endif /* CONFIG_BRIGHTNESS_ADJUSTMENT_AUTOMATIC */

#if CONFIG_BLINKING
/**
 * @rst
 *
 * .. test:: light_controller.test_light_blinking
 *    :id: TS_LC-002
 *    :tests: SWDD_LC-101
 *
 * @endrst
 */
TEST(light_controller, test_light_blinking)
{
    /*!
     * Test case for checking the blinking state changes while power is ON.
     */
    CREATE_MOCK(mymock);

    // Set the initial power state to ON so the light can blink
    EXPECT_CALL(mymock, RteGetPowerState()).WillRepeatedly(Return(POWER_STATE_ON));

    // Assume main knob value is 50, so blinkPeriod will be 50.
    EXPECT_CALL(mymock, RteGetMainKnobValue()).WillRepeatedly(Return(50));

    // Expect that the light value changes to ON color (only once, no redundant updates).
    EXPECT_CALL(mymock, RteSetLightValue(RGBColorEq(onColor))).Times(1);
    lightController();

    // Call lightController repeatedly to simulate time passing and check the light state
    for (int i = 0; i < 49; i++)
    {
        EXPECT_CALL(mymock, RteSetLightValue(_)).Times(0);
        lightController();
    }

    // Expect that the light value changes to OFF color (only once, no redundant updates).
    EXPECT_CALL(mymock, RteSetLightValue(RGBColorEq(offColor))).Times(1);
    lightController();

    // Call lightController repeatedly to simulate time passing and check the light state
    for (int i = 0; i < 49; i++)
    {
        EXPECT_CALL(mymock, RteSetLightValue(_)).Times(0);
        lightController();
    }

    // Expect that the light value changes to ON color (only once, no redundant updates).
    EXPECT_CALL(mymock, RteSetLightValue(RGBColorEq(onColor))).Times(1);
    lightController();
}

// Define a test fixture class
class BlinkPeriodTest : public TestWithParam<struct TestParam>
{
};

// Define a struct to hold the parameters for each test case
struct TestParam
{
    const char *description;
    percentage_t mainKnobValue;
    unsigned int expectedBlinkPeriod;
};

// Override the cout operator for TestParam so that it can be printed in the test output
std::ostream &operator<<(std::ostream &os, const TestParam &param)
{
    os << param.description;
    return os;
}

/**
 * @rst
 *
 * .. test:: light_controller.test_correct_blink_period
 *    :id: TS_LC-004
 *    :tests: SWDD_LC-101
 *
 * @endrst
 */
TEST(light_controller, test_correct_blink_period)
{
    /**
     * Test cases for the calculateBlinkPeriod function.
     */
    std::vector<TestParam> test_data = {
        {"Slowest", 0, 100},
        {"Inbetween", 50, 50},
        {"Fastest", 100, 10}};

    for (const auto &param : test_data)
    {
        unsigned int blinkPeriod = calculateBlinkPeriod(param.mainKnobValue);
        EXPECT_EQ(blinkPeriod, param.expectedBlinkPeriod) << "Test case: " << param.description;
    }
}

/**
 * @rst
 *
 * .. test:: BlinkPeriodTests/BlinkPeriodTest.CalculatesCorrectBlinkPeriod/*
 *    :id: TS_LC-003
 *    :tests: SWDD_LC-101
 *
 * @endrst
 */
TEST_P(BlinkPeriodTest, CalculatesCorrectBlinkPeriod)
{
    // Get the parameters for this test case
    TestParam param = GetParam();

    unsigned int blinkPeriod = calculateBlinkPeriod(param.mainKnobValue);
    EXPECT_EQ(blinkPeriod, param.expectedBlinkPeriod) << "Test case: " << param.description;
}

// Instantiate the test suite with a set of parameters
INSTANTIATE_TEST_SUITE_P(
    BlinkPeriodTests,
    BlinkPeriodTest,
    Values(
        TestParam{"Slowest", 0, 100},
        TestParam{"Inbetween", 50, 50},
        TestParam{"Fastest", 100, 10}));

#endif // CONFIG_BLINKING
