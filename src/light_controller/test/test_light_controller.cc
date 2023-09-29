/**
 * @file
 */
#include <gtest/gtest.h>
using namespace testing;

extern "C" {
#include "light_controller.h"
#include "rte.h"
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


/*!
* @rst
*
* .. test:: light_controller.test_light_on_and_off
*    :id: TS_LC-001
*    :results: [[tr_link('title', 'case')]]
*    :specified: SWDD_LC-001, SWDD_LC-002
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
    RGBColor onColor = { .red = 0, .green = 128, .blue = 55 };
    // Expect that the light value changes to ON color (only once, no redundant updates).
    EXPECT_CALL(mymock, RteSetLightValue(RGBColorEq(onColor))).Times(1);
    for (int i = 0; i < 10; i++) {
        lightController();
    }

    // Power turns OFF, so the light should turn OFF.
    EXPECT_CALL(mymock, RteGetPowerState()).WillRepeatedly(Return(POWER_STATE_OFF));
    RGBColor offColor = { .red = 0, .green = 0, .blue = 0 };
    // Expect that the light value changes to OFF color (only once, no redundant updates).
    EXPECT_CALL(mymock, RteSetLightValue(RGBColorEq(offColor))).Times(1);
    for (int i = 0; i < 10; i++) {
        lightController();
    }
}
