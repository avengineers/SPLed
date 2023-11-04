/**
 * @file
 */

#include <gtest/gtest.h>
using namespace testing;

extern "C" {
#include "keyboard_interface.h"
#include "rte.h"
}

#include "mockup_src_keyboard_interface.h"

/*!
* @rst
*
* .. test:: keyboard_interface.test_key_press_and_release
*    :id: TS_KI-001
*    :tests: SWDD_KI-001, SWDD_KI-002, SWDD_KI-003
*
* @endrst
*/
TEST(keyboard_interface, test_key_press_and_release)
{
    CREATE_MOCK(mymock);

    // Key is initially not pressed
    EXPECT_CALL(mymock, RteIsKeyPressed(TARGET_KEY)).WillOnce(Return(FALSE));
    EXPECT_CALL(mymock, RteSetPowerKeyPressed(FALSE)).Times(1);
    keyboardInterface();

    // Key is pressed but not yet debounced
    for (unsigned int i = 0; i < CFG_DEBOUNCE_PRESS - 1; i++) {
        EXPECT_CALL(mymock, RteIsKeyPressed(TARGET_KEY)).WillOnce(Return(TRUE));
        EXPECT_CALL(mymock, RteSetPowerKeyPressed(FALSE)).Times(1);
        keyboardInterface();
    }

    // Key is pressed and debounced
    EXPECT_CALL(mymock, RteIsKeyPressed(TARGET_KEY)).WillOnce(Return(TRUE));
    EXPECT_CALL(mymock, RteSetPowerKeyPressed(TRUE)).Times(1);
    keyboardInterface();

    // Key remains pressed, but the function should not be triggered again until debounce
    for (unsigned int i = 0; i < 2*CFG_DEBOUNCE_PRESS; i++) {
        EXPECT_CALL(mymock, RteIsKeyPressed(TARGET_KEY)).WillOnce(Return(TRUE));
        EXPECT_CALL(mymock, RteSetPowerKeyPressed(FALSE)).Times(1);
        keyboardInterface();
    }

    // Key is released but not yet debounced
    for (unsigned int i = 0; i < CFG_DEBOUNCE_RELEASE - 1; i++) {
        EXPECT_CALL(mymock, RteIsKeyPressed(TARGET_KEY)).WillOnce(Return(FALSE));
        EXPECT_CALL(mymock, RteSetPowerKeyPressed(FALSE)).Times(1);
        keyboardInterface();
    }

    // Key is released and debounced
    EXPECT_CALL(mymock, RteIsKeyPressed(TARGET_KEY)).WillOnce(Return(FALSE));
    EXPECT_CALL(mymock, RteSetPowerKeyPressed(FALSE)).Times(1);
    keyboardInterface();

    // Key remains released, but the function should not be triggered again until debounce
    for (unsigned int i = 0; i < 2*CFG_DEBOUNCE_RELEASE; i++) {
        EXPECT_CALL(mymock, RteIsKeyPressed(TARGET_KEY)).WillOnce(Return(FALSE));
        EXPECT_CALL(mymock, RteSetPowerKeyPressed(FALSE)).Times(1);
        keyboardInterface();
    }

    // Key is pressed again but not yet debounced
    for (unsigned int i = 0; i < CFG_DEBOUNCE_PRESS - 1; i++) {
        EXPECT_CALL(mymock, RteIsKeyPressed(TARGET_KEY)).WillOnce(Return(TRUE));
        EXPECT_CALL(mymock, RteSetPowerKeyPressed(FALSE)).Times(1);
        keyboardInterface();
    }

    // Key is pressed and debounced
    EXPECT_CALL(mymock, RteIsKeyPressed(TARGET_KEY)).WillOnce(Return(TRUE));
    EXPECT_CALL(mymock, RteSetPowerKeyPressed(TRUE)).Times(1);
    keyboardInterface();
}
