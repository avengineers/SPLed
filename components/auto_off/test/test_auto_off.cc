/**
 * @file test_auto_off.cc
 * @brief Unit tests for the auto off component.
 */

#include <gtest/gtest.h>
#include <gmock/gmock.h>
using namespace testing;

extern "C"
{
#include "autoconf.h"
#include "auto_off.h"
#include "rte.h"

    bool_t autoOffIsUserActive(void);
    extern uint32_t autoOffTimer;
}

#include "mockup_components_auto_off.h"

/** The configured auto off period, in the millisecond unit the timer uses. */
static const uint32_t AUTO_OFF_TEST_PERIOD_MS = ((uint32_t)CONFIG_AUTO_OFF_PERIOD_SECONDS * 1000u);
/** One execution of the component, in milliseconds. */
static const uint32_t AUTO_OFF_TEST_TICK_MS = (uint32_t)CONFIG_OS_TASK_PERIOD;

struct KeyActivityParams
{
    bool_t powerKeyPressed;
    bool_t upKeyPressed;
    bool_t downKeyPressed;
    bool_t expectedActive;
    const char *description;
};

inline std::ostream &operator<<(std::ostream &os, const KeyActivityParams &param)
{
    os << param.description;
    return os;
}

class AutoOffActivityTest : public TestWithParam<KeyActivityParams>
{
};

INSTANTIATE_TEST_SUITE_P(
    KeyCombinations,
    AutoOffActivityTest,
    Values(
        KeyActivityParams{FALSE, FALSE, FALSE, FALSE, "No key pressed is no activity"},
        KeyActivityParams{TRUE, FALSE, FALSE, TRUE, "Power key alone is activity"},
        KeyActivityParams{FALSE, TRUE, FALSE, TRUE, "Up key alone is activity"},
        KeyActivityParams{FALSE, FALSE, TRUE, TRUE, "Down key alone is activity"},
        KeyActivityParams{TRUE, TRUE, TRUE, TRUE, "All keys together are activity"}));

/**
 * @rst
 * .. test:: KeyCombinations/AutoOffActivityTest.DetectsUserActivity
 *    :id: TS_AO-001
 *    :tests: SWDD_AO-111, SWDD_AO-211
 *    :results: [[links_from_filter("type == 'testcase' and search('^KeyCombinations/AutoOffActivityTest.DetectsUserActivity/', case)")]]
 *
 * A press of the power button, the up key or the down key is user activity, and nothing else is.
 * Each key is read at most once, because the check may stop at the first press it finds.
 * @endrst
 */
TEST_P(AutoOffActivityTest, DetectsUserActivity)
{
    // ARRANGE
    const auto &param = GetParam();
    CREATE_MOCK(mymock);

    EXPECT_CALL(mymock, RteIsKeyPressed(POWER_BUTTON_KEY)).Times(AtMost(1)).WillRepeatedly(Return(param.powerKeyPressed));
    EXPECT_CALL(mymock, RteIsKeyPressed(CONTROL_KEY_UP)).Times(AtMost(1)).WillRepeatedly(Return(param.upKeyPressed));
    EXPECT_CALL(mymock, RteIsKeyPressed(CONTROL_KEY_DOWN)).Times(AtMost(1)).WillRepeatedly(Return(param.downKeyPressed));

    // ACT
    const bool_t isActive = autoOffIsUserActive();

    // ASSERT
    EXPECT_EQ(isActive, param.expectedActive);
}

/**
 * @rst
 * .. test:: auto_off.ResetsTimerToFullPeriod_When_UserIsActive
 *    :id: TS_AO-002
 *    :tests: SWDD_AO-112, SWDD_AO-114, SWDD_AO-210, SWDD_AO-212
 *
 * A key press reloads the timer with the configured period, whatever it held before, and the
 * reported state goes back to FALSE.
 * @endrst
 */
TEST(auto_off, ResetsTimerToFullPeriod_When_UserIsActive)
{
    // ARRANGE
    autoOffTimer = 0u;
    CREATE_MOCK(mymock);

    EXPECT_CALL(mymock, RteIsKeyPressed(POWER_BUTTON_KEY)).Times(AtMost(1)).WillRepeatedly(Return(TRUE));
    EXPECT_CALL(mymock, RteIsKeyPressed(CONTROL_KEY_UP)).Times(AtMost(1)).WillRepeatedly(Return(FALSE));
    EXPECT_CALL(mymock, RteIsKeyPressed(CONTROL_KEY_DOWN)).Times(AtMost(1)).WillRepeatedly(Return(FALSE));
    EXPECT_CALL(mymock, RteSetAutoOffState(FALSE)).Times(1);

    // ACT
    autoOff();

    // ASSERT
    EXPECT_EQ(autoOffTimer, AUTO_OFF_TEST_PERIOD_MS);
}

/**
 * @rst
 * .. test:: auto_off.ReloadsTimerToFullPeriod_When_UserIsActiveAndTimerIsPartlySpent
 *    :id: TS_AO-007
 *    :tests: SWDD_AO-114
 *
 * SWDD_AO-114 reloads the timer "regardless of its previous value". Starting from a partly spent
 * timer pins that down: an implementation that added the period instead of assigning it would
 * grow the timeout without bound while the user keeps pressing keys, and TS_AO-002 alone cannot
 * see the difference because it starts from zero.
 * @endrst
 */
TEST(auto_off, ReloadsTimerToFullPeriod_When_UserIsActiveAndTimerIsPartlySpent)
{
    // ARRANGE
    autoOffTimer = AUTO_OFF_TEST_PERIOD_MS / 2u;
    CREATE_MOCK(mymock);

    EXPECT_CALL(mymock, RteIsKeyPressed(POWER_BUTTON_KEY)).Times(AtMost(1)).WillRepeatedly(Return(FALSE));
    EXPECT_CALL(mymock, RteIsKeyPressed(CONTROL_KEY_UP)).Times(AtMost(1)).WillRepeatedly(Return(TRUE));
    EXPECT_CALL(mymock, RteIsKeyPressed(CONTROL_KEY_DOWN)).Times(AtMost(1)).WillRepeatedly(Return(FALSE));
    EXPECT_CALL(mymock, RteSetAutoOffState(FALSE)).Times(1);

    // ACT
    autoOff();

    // ASSERT
    EXPECT_EQ(autoOffTimer, AUTO_OFF_TEST_PERIOD_MS);
}

/**
 * @rst
 * .. test:: auto_off.CountsTimerDown_When_UserIsInactive
 *    :id: TS_AO-003
 *    :tests: SWDD_AO-113
 *
 * Without a key press the timer loses one task period per execution and the state stays FALSE.
 * @endrst
 */
TEST(auto_off, CountsTimerDown_When_UserIsInactive)
{
    // ARRANGE
    autoOffTimer = AUTO_OFF_TEST_PERIOD_MS;
    CREATE_MOCK(mymock);

    EXPECT_CALL(mymock, RteIsKeyPressed(_)).Times(3).WillRepeatedly(Return(FALSE));
    EXPECT_CALL(mymock, RteSetAutoOffState(FALSE)).Times(1);

    // ACT
    autoOff();

    // ASSERT
    EXPECT_EQ(autoOffTimer, AUTO_OFF_TEST_PERIOD_MS - AUTO_OFF_TEST_TICK_MS);
}

/**
 * @rst
 * .. test:: auto_off.SaturatesTimerAtZero_When_LessThanOneTaskPeriodIsLeft
 *    :id: TS_AO-004
 *    :tests: SWDD_AO-110, SWDD_AO-113
 *
 * A timer holding less than one task period reaches zero instead of wrapping around, and the
 * component reports the auto off state.
 * @endrst
 */
TEST(auto_off, SaturatesTimerAtZero_When_LessThanOneTaskPeriodIsLeft)
{
    // ARRANGE
    autoOffTimer = AUTO_OFF_TEST_TICK_MS - 1u;
    CREATE_MOCK(mymock);

    EXPECT_CALL(mymock, RteIsKeyPressed(_)).Times(3).WillRepeatedly(Return(FALSE));
    EXPECT_CALL(mymock, RteSetAutoOffState(TRUE)).Times(1);

    // ACT
    autoOff();

    // ASSERT
    EXPECT_EQ(autoOffTimer, 0u);
}

/**
 * @rst
 * .. test:: auto_off.ReportsAutoOff_When_TimerIsZero
 *    :id: TS_AO-005
 *    :tests: SWDD_AO-110, SWDD_AO-115, SWDD_AO-212
 *
 * Zero is also the timer's value before the first user activity, so this is what the component
 * reports after a reset: the auto off state, held for as long as nobody touches a key.
 * @endrst
 */
TEST(auto_off, ReportsAutoOff_When_TimerIsZero)
{
    // ARRANGE
    autoOffTimer = 0u;
    CREATE_MOCK(mymock);

    EXPECT_CALL(mymock, RteIsKeyPressed(_)).Times(3).WillRepeatedly(Return(FALSE));
    EXPECT_CALL(mymock, RteSetAutoOffState(TRUE)).Times(1);

    // ACT
    autoOff();

    // ASSERT
    EXPECT_EQ(autoOffTimer, 0u);
}

/**
 * @rst
 * .. test:: auto_off.ReportsAutoOff_After_TheConfiguredPeriod
 *    :id: TS_AO-006
 *    :tests: SWDD_AO-110, SWDD_AO-112, SWDD_AO-310, SWDD_AO-311
 *
 * From a full timer, the state turns TRUE on the execution that completes the configured period,
 * and on no earlier one.
 * @endrst
 */
TEST(auto_off, ReportsAutoOff_After_TheConfiguredPeriod)
{
    // ARRANGE
    const uint32_t executionsPerPeriod = AUTO_OFF_TEST_PERIOD_MS / AUTO_OFF_TEST_TICK_MS;
    autoOffTimer = AUTO_OFF_TEST_PERIOD_MS;
    CREATE_MOCK(mymock);

    EXPECT_CALL(mymock, RteIsKeyPressed(_)).Times(AnyNumber()).WillRepeatedly(Return(FALSE));
    EXPECT_CALL(mymock, RteSetAutoOffState(FALSE)).Times(executionsPerPeriod - 1u);
    EXPECT_CALL(mymock, RteSetAutoOffState(TRUE)).Times(1);

    // ACT
    for (uint32_t execution = 0u; execution < executionsPerPeriod; execution++)
    {
        autoOff();
    }

    // ASSERT
    EXPECT_EQ(autoOffTimer, 0u);
}
