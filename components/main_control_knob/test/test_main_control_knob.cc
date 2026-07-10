/**
 * @file
 */

#include <gtest/gtest.h>
using namespace testing;

extern "C"
{
#include "main_control_knob.h"
}

#include "mockup_components_main_control_knob.h"

// Define a struct to hold the parameters for testing mainControlKnob
struct MainControlKnobParameters
{
    const char *description;
    bool arrowUpPressed;
    bool arrowDownPressed;
    int initialKnobValue;
    int expectedKnobValue;
};

// Override the cout operator for TestParam so that it can be printed in the test output
std::ostream &operator<<(std::ostream &os, const MainControlKnobParameters &param)
{
    os << param.description;
    return os;
}

// Define a test fixture class
class MainControlKnobTest : public TestWithParam<struct MainControlKnobParameters>
{
};

/**
 * @rst
 * .. test:: MainControlKnobTests/MainControlKnobTest.HandleKnobInput/*
 *    :id: TS_MCK-001
 *    :tests: SWDD_MCK-100, SWDD_MCK-101, SWDD_MCK-200, SWDD_MCK-201, SWDD_MCK-202, SWDD_MCK-203
 * @endrst
 */
TEST_P(MainControlKnobTest, HandleKnobInput)
{
    /* Arrange */
    MainControlKnobParameters param = GetParam();

    CREATE_MOCK(mymock);

    EXPECT_CALL(mymock, RteIsKeyPressed(0x26)).WillOnce(Return(param.arrowUpPressed)); // Arrow Up
    if (param.arrowUpPressed)
    {
        EXPECT_CALL(mymock, RteIsKeyPressed(0x28)).Times(0); // Arrow Down should not be pressed
    }
    else
    {
        EXPECT_CALL(mymock, RteIsKeyPressed(0x28)).WillOnce(Return(param.arrowDownPressed)); // Arrow Down
    }
    EXPECT_CALL(mymock, RteGetMainKnobValue()).WillOnce(Return(param.initialKnobValue));
    EXPECT_CALL(mymock, RteSetMainKnobValue(param.expectedKnobValue));

    /* Act */
    mainControlKnob();
}

// Instantiate the test suite with a set of parameters
INSTANTIATE_TEST_SUITE_P(
    MainControlKnobTests,
    MainControlKnobTest,
    Values(
        MainControlKnobParameters{"Arrow Up increases value", true, false, 50, 52},
        MainControlKnobParameters{"Arrow Down decreases value", false, true, 50, 48},
        MainControlKnobParameters{"Does not decrease below zero", false, true, 1, 0},
        MainControlKnobParameters{"Does not decrease below zero if already zero", false, true, 0, 0},
        MainControlKnobParameters{"Does not increase over 100", true, false, 99, 100},
        MainControlKnobParameters{"Does not increase over 100 if already 100", true, false, 100, 100},
        MainControlKnobParameters{"No key pressed, value remains the same", false, false, 50, 50}));
