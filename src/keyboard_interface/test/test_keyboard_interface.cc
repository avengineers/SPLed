/**
 * @file test_keyboard_interface.cc
 * @brief GTest file for the keyboard interface module.
 */

#include <gtest/gtest.h>
using namespace testing;

extern "C" {
#include "keyboard_interface.h"
}

#include "mockup_src_keyboard_interface.h"


/*!
* @rst
*
* .. test:: keyboard_interface.test_key_press_and_release
*    :id: TS_KI-001
*    :results: [[tr_link('title', 'case')]]
*    :specified: SWDD_KI-001, SWDD_KI-002, SWDD_KI-003
*
* @endrst
*/
TEST(keyboard_interface, test_key_press_and_release)
{
    CREATE_MOCK(mymock);

    // Key is initially not pressed
    EXPECT_CALL(mymock, RteIsKeyPressed(TARGET_KEY)).WillOnce(Return(FALSE));
    EXPECT_CALL(mymock, RteSetPowerKeyPressed(_)).Times(0);
    keyboardInterface();

    // Key is pressed
    EXPECT_CALL(mymock, RteIsKeyPressed(TARGET_KEY)).WillOnce(Return(TRUE));
    EXPECT_CALL(mymock, RteSetPowerKeyPressed(TRUE));
    keyboardInterface();

    // Key is still pressed, no state change
    EXPECT_CALL(mymock, RteIsKeyPressed(TARGET_KEY)).WillOnce(Return(TRUE));
    EXPECT_CALL(mymock, RteSetPowerKeyPressed(_)).Times(0);
    keyboardInterface();

    // Key is released
    EXPECT_CALL(mymock, RteIsKeyPressed(TARGET_KEY)).WillOnce(Return(FALSE));
    EXPECT_CALL(mymock, RteSetPowerKeyPressed(FALSE));
    keyboardInterface();

    // Key is still released, no state change
    EXPECT_CALL(mymock, RteIsKeyPressed(TARGET_KEY)).WillOnce(Return(FALSE));
    EXPECT_CALL(mymock, RteSetPowerKeyPressed(_)).Times(0);
    keyboardInterface();

    // Key is pressed again
    EXPECT_CALL(mymock, RteIsKeyPressed(TARGET_KEY)).WillOnce(Return(TRUE));
    EXPECT_CALL(mymock, RteSetPowerKeyPressed(TRUE));
    keyboardInterface();

}
