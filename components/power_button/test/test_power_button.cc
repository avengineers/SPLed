/**
 * @file
 */

#include <gtest/gtest.h>
using namespace testing;

extern "C"
{
#include "power_button.h"
#include "rte.h"
}

#include "mockup_components_power_button.h"

class PowerButtonTest : public Test
{
protected:
    void SetUp() override
    {
        powerButtonInit();
    }
};

/*!
 * @rst
 *
 * .. test:: PowerButtonTest.InitialStateIsReleased
 *    :id: TS_PB-001
 *    :tests: SWDD_PB-100, SWDD_PB-201, SWDD_PB-203
 *
 * @endrst
 */
TEST_F(PowerButtonTest, InitialStateIsReleased)
{
    CREATE_MOCK(mymock);

    // In the initial state, the key is considered released, and no event should be sent.
    EXPECT_CALL(mymock, RteIsKeyPressed(POWER_BUTTON_KEY)).WillOnce(Return(FALSE));
    EXPECT_CALL(mymock, RteSetPowerKeyPressedEvent(FALSE)).Times(1);
    powerButton();
}

/*!
 * @rst
 *
 * .. test:: PowerButtonTest.PressEventSentOnlyAfterDebounce
 *    :id: TS_PB-002
 *    :tests: SWDD_PB-101, SWDD_PB-203
 *
 * @endrst
 */
TEST_F(PowerButtonTest, PressEventSentOnlyAfterDebounce)
{
    CREATE_MOCK(mymock);

    // Simulate key press for less than the debounce count
    for (unsigned int i = 0; i < POWER_BUTTON_PRESS_DEBOUNCE - 1; i++)
    {
        EXPECT_CALL(mymock, RteIsKeyPressed(POWER_BUTTON_KEY)).WillOnce(Return(TRUE));
        EXPECT_CALL(mymock, RteSetPowerKeyPressedEvent(FALSE)).Times(1);
        powerButton();
    }

    // The next press should trigger the event
    EXPECT_CALL(mymock, RteIsKeyPressed(POWER_BUTTON_KEY)).WillOnce(Return(TRUE));
    EXPECT_CALL(mymock, RteSetPowerKeyPressedEvent(TRUE)).Times(1);
    powerButton();
}

/*!
 * @rst
 *
 * .. test:: PowerButtonTest.FullPressAndReleaseCycle
 *    :id: TS_PB-003
 *    :tests: SWDD_PB-100, SWDD_PB-101, SWDD_PB-200, SWDD_PB-202, SWDD_PB-203, SWDD_PB-300
 *
 * @endrst
 */
TEST_F(PowerButtonTest, FullPressAndReleaseCycle)
{
    CREATE_MOCK(mymock);

    // Key is initially not pressed
    EXPECT_CALL(mymock, RteIsKeyPressed(POWER_BUTTON_KEY)).WillOnce(Return(FALSE));
    EXPECT_CALL(mymock, RteSetPowerKeyPressedEvent(FALSE)).Times(1);
    powerButton();

    // Key is pressed but not yet debounced
    for (unsigned int i = 0; i < POWER_BUTTON_PRESS_DEBOUNCE - 1; i++)
    {
        EXPECT_CALL(mymock, RteIsKeyPressed(POWER_BUTTON_KEY)).WillOnce(Return(TRUE));
        EXPECT_CALL(mymock, RteSetPowerKeyPressedEvent(FALSE)).Times(1);
        powerButton();
    }

    // Key is pressed and debounced
    EXPECT_CALL(mymock, RteIsKeyPressed(POWER_BUTTON_KEY)).WillOnce(Return(TRUE));
    EXPECT_CALL(mymock, RteSetPowerKeyPressedEvent(TRUE)).Times(1);
    powerButton();

    // Key remains pressed, but the function should not be triggered again until debounce
    for (unsigned int i = 0; i < 2 * POWER_BUTTON_PRESS_DEBOUNCE; i++)
    {
        EXPECT_CALL(mymock, RteIsKeyPressed(POWER_BUTTON_KEY)).WillOnce(Return(TRUE));
        EXPECT_CALL(mymock, RteSetPowerKeyPressedEvent(FALSE)).Times(1);
        powerButton();
    }

    // Key is released but not yet debounced
    for (unsigned int i = 0; i < POWER_BUTTON_RELEASE_DEBOUNCE - 1; i++)
    {
        EXPECT_CALL(mymock, RteIsKeyPressed(POWER_BUTTON_KEY)).WillOnce(Return(FALSE));
        EXPECT_CALL(mymock, RteSetPowerKeyPressedEvent(FALSE)).Times(1);
        powerButton();
    }

    // Key is released and debounced
    EXPECT_CALL(mymock, RteIsKeyPressed(POWER_BUTTON_KEY)).WillOnce(Return(FALSE));
    EXPECT_CALL(mymock, RteSetPowerKeyPressedEvent(FALSE)).Times(1);
    powerButton();

    // Key remains released, but the function should not be triggered again until debounce
    for (unsigned int i = 0; i < 2 * POWER_BUTTON_RELEASE_DEBOUNCE; i++)
    {
        EXPECT_CALL(mymock, RteIsKeyPressed(POWER_BUTTON_KEY)).WillOnce(Return(FALSE));
        EXPECT_CALL(mymock, RteSetPowerKeyPressedEvent(FALSE)).Times(1);
        powerButton();
    }

    // Key is pressed again but not yet debounced
    for (unsigned int i = 0; i < POWER_BUTTON_PRESS_DEBOUNCE - 1; i++)
    {
        EXPECT_CALL(mymock, RteIsKeyPressed(POWER_BUTTON_KEY)).WillOnce(Return(TRUE));
        EXPECT_CALL(mymock, RteSetPowerKeyPressedEvent(FALSE)).Times(1);
        powerButton();
    }

    // Key is pressed and debounced
    EXPECT_CALL(mymock, RteIsKeyPressed(POWER_BUTTON_KEY)).WillOnce(Return(TRUE));
    EXPECT_CALL(mymock, RteSetPowerKeyPressedEvent(TRUE)).Times(1);
    powerButton();
}

/*!
 * @rst
 *
 * .. test:: PowerButtonTest.InitToReleasedTransition
 *    :id: TS_PB-004
 *    :tests: SWDD_PB-100, SWDD_PB-101, SWDD_PB-300
 *
 * @endrst
 */
TEST_F(PowerButtonTest, InitToReleasedTransition)
{
    CREATE_MOCK(mymock);

    // Simulate key being released for the debounce period to transition from INIT to RELEASED
    for (unsigned int i = 0; i < POWER_BUTTON_RELEASE_DEBOUNCE; i++)
    {
        EXPECT_CALL(mymock, RteIsKeyPressed(POWER_BUTTON_KEY)).WillOnce(Return(FALSE));
        EXPECT_CALL(mymock, RteSetPowerKeyPressedEvent(FALSE)).Times(1);
        powerButton();
    }

    // At this point, the state machine should be in the RELEASED state.
    // A subsequent press should now trigger a transition to the PRESSED state.
    for (unsigned int i = 0; i < POWER_BUTTON_PRESS_DEBOUNCE - 1; i++)
    {
        EXPECT_CALL(mymock, RteIsKeyPressed(POWER_BUTTON_KEY)).WillOnce(Return(TRUE));
        EXPECT_CALL(mymock, RteSetPowerKeyPressedEvent(FALSE)).Times(1);
        powerButton();
    }

    EXPECT_CALL(mymock, RteIsKeyPressed(POWER_BUTTON_KEY)).WillOnce(Return(TRUE));
    EXPECT_CALL(mymock, RteSetPowerKeyPressedEvent(TRUE)).Times(1);
    powerButton();
}
