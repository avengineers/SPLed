/**
 * @file
 */

#include <gtest/gtest.h>
using namespace testing;

extern "C" {
#include "main_control_knob.h"
}

#include "mockup_src_main_control_knob.h"


/*!
* @rst
*
* .. test:: main_control_knob.test_main_arrow_up_increases_brightness
*    :id: TS_MCK-001
*    :results: [[tr_link('title', 'case')]]
*    :specified: SWDD_MCK-001
*
* @endrst
*/
TEST(main_control_knob, test_main_arrow_up_increases_brightness)
{
    CREATE_MOCK(mymock);

    // Simulate Arrow Up key press.
    EXPECT_CALL(mymock, RteIsKeyPressed(0x26)).WillOnce(Return(TRUE));
    // Set initial knob value
    EXPECT_CALL(mymock, RteGetMainKnobValue()).WillOnce(Return(50));
    // Verify that knob value was increased.
    EXPECT_CALL(mymock, RteSetMainKnobValue(52));

    // Call the function to handle knob input.
    knobControlInput();

}



/*!
* @rst
*
* .. test:: main_control_knob.test_main_arrow_down_decreases_brightness
*    :id: TS_MCK-002
*    :results: [[tr_link('title', 'case')]]
*    :specified: SWDD_MCK-002
*
* @endrst
*/
TEST(main_control_knob, test_main_arrow_down_decreases_brightness)
{
    CREATE_MOCK(mymock);

    // Simulate Arrow Down key press.
    EXPECT_CALL(mymock, RteIsKeyPressed(0x26)).WillOnce(Return(FALSE)); // Arrow Up not pressed.
    EXPECT_CALL(mymock, RteIsKeyPressed(0x28)).WillOnce(Return(TRUE)); // Arrow Down pressed.
    // Set initial knob value
    EXPECT_CALL(mymock, RteGetMainKnobValue()).WillOnce(Return(50));
    // Verify that knob value was decreased.
    EXPECT_CALL(mymock, RteSetMainKnobValue(48));

    // Call the function to handle knob input.
    knobControlInput();

}

/*!
* @rst
*
* .. test:: main_control_knob.test_main_does_not_decrease_bellow_zero
*    :id: TS_MCK-003
*    :results: [[tr_link('title', 'case')]]
*    :specified: SWDD_MCK-002
*
* @endrst
*/
TEST(main_control_knob, test_main_does_not_decrease_bellow_zero)
{
    CREATE_MOCK(mymock);

    // Simulate Arrow Down key press.
    EXPECT_CALL(mymock, RteIsKeyPressed(0x26)).WillOnce(Return(FALSE)); // Arrow Up not pressed.
    EXPECT_CALL(mymock, RteIsKeyPressed(0x28)).WillOnce(Return(TRUE)); // Arrow Down pressed.
    // Set initial knob value
    EXPECT_CALL(mymock, RteGetMainKnobValue()).WillOnce(Return(1));
    // Verify that knob value was decreased.
    EXPECT_CALL(mymock, RteSetMainKnobValue(0));

    // Call the function to handle knob input.
    knobControlInput();

}

/*!
* @rst
*
* .. test:: main_control_knob.test_main_does_not_increase_over_one_hundred
*    :id: TS_MCK-004
*    :results: [[tr_link('title', 'case')]]
*    :specified: SWDD_MCK-002
*
* @endrst
*/
TEST(main_control_knob, test_main_does_not_increase_over_one_hundred)
{
    CREATE_MOCK(mymock);

    // Simulate Arrow Up key press.
    EXPECT_CALL(mymock, RteIsKeyPressed(0x26)).WillOnce(Return(TRUE)); // Arrow Up pressed.
    // Set initial knob value
    EXPECT_CALL(mymock, RteGetMainKnobValue()).WillOnce(Return(99));
    // Verify that knob value was decreased.
    EXPECT_CALL(mymock, RteSetMainKnobValue(100));

    // Call the function to handle knob input.
    knobControlInput();

}
