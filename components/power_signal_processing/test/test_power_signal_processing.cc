/**
 * @file
 */

#include <gtest/gtest.h>
using namespace testing;

extern "C"
{
#include "power_signal_processing.h"
}

#include "mockup_components_power_signal_processing.h"

/*!
 * @rst
 *
 * .. test:: power_signal_processing.test_power_stays_off
 *    :id: TS_PSP-001
 *    :tests: SWDD_PSP-001
 *
 * @endrst
 */
TEST(power_signal_processing, test_power_stays_off)
{
    CREATE_MOCK(mymock);

    EXPECT_CALL(mymock, RteGetPowerState()).Times(0);
    EXPECT_CALL(mymock, RteSetPowerState(_)).Times(0);
    EXPECT_CALL(mymock, RteGetPowerKeyPressedEvent()).Times(1).WillOnce(Return(FALSE));

    powerSignalProcessing();
}

/*!
 * @rst
 *
 * .. test:: power_signal_processing.test_power_toggles
 *    :id: TS_PSP-002
 *    :tests: SWDD_PSP-002, SWDD_PSP-003
 *
 * @endrst
 */
TEST(power_signal_processing, test_power_toggles)
{
    CREATE_MOCK(mymock); // Assuming you have a mock creation macro

    // Starting with power state off and P key pressed once
    EXPECT_CALL(mymock, RteGetPowerState()).WillOnce(Return(POWER_STATE_OFF));
    EXPECT_CALL(mymock, RteGetPowerKeyPressedEvent()).WillOnce(Return(TRUE));
    EXPECT_CALL(mymock, RteSetPowerState(POWER_STATE_ON));
    powerSignalProcessing();

    // Press Power key again to toggle power state to off
    EXPECT_CALL(mymock, RteGetPowerState()).WillOnce(Return(POWER_STATE_ON));
    EXPECT_CALL(mymock, RteGetPowerKeyPressedEvent()).WillOnce(Return(TRUE));
    EXPECT_CALL(mymock, RteSetPowerState(POWER_STATE_OFF));
    powerSignalProcessing();

    // Press Power key once more to toggle power state back to on
    EXPECT_CALL(mymock, RteGetPowerState()).WillOnce(Return(POWER_STATE_OFF));
    EXPECT_CALL(mymock, RteGetPowerKeyPressedEvent()).WillOnce(Return(TRUE));
    EXPECT_CALL(mymock, RteSetPowerState(POWER_STATE_ON));
    powerSignalProcessing();
}

#ifdef CONFIG_AUTO_OFF
/*!
 * @rst
 *
 * .. test:: power_signal_processing.test_auto_off_event_powers_down
 *    :id: TS_PSP-003
 *    :tests: SWDD_PSP-004
 *
 * Test that when auto off event occurs, the system is powered down.
 *
 * @endrst
 */
TEST(power_signal_processing, test_auto_off_event_powers_down)
{
    CREATE_MOCK(mymock);

    // Auto off event triggered, no power key pressed
    EXPECT_CALL(mymock, RteGetPowerKeyPressedEvent()).WillOnce(Return(FALSE));
    EXPECT_CALL(mymock, RteGetAutoOffState()).WillOnce(Return(TRUE));
    EXPECT_CALL(mymock, RteSetPowerState(POWER_STATE_OFF));

    powerSignalProcessing();
}

/*!
 * @rst
 *
 * .. test:: power_signal_processing.test_no_auto_off_event_no_action
 *    :id: TS_PSP-004
 *    :tests: SWDD_PSP-005
 *
 * Test that when no auto off event occurs and no power key is pressed, no action is taken.
 *
 * @endrst
 */
TEST(power_signal_processing, test_no_auto_off_event_no_action)
{
    CREATE_MOCK(mymock);

    // No power key pressed and no auto off event
    EXPECT_CALL(mymock, RteGetPowerKeyPressedEvent()).WillOnce(Return(FALSE));
    EXPECT_CALL(mymock, RteGetAutoOffState()).WillOnce(Return(FALSE));
    EXPECT_CALL(mymock, RteSetPowerState(_)).Times(0);

    powerSignalProcessing();
}
#endif // CONFIG_AUTO_OFF
