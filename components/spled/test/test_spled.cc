#include <gtest/gtest.h>
using namespace testing;

extern "C"
{
#include "autoconf.h"
#include "spled.h"
}

#include "mockup_components_spled.h"

TEST(spled, test_interface_call_order)
{
   CREATE_MOCK(mymock);

   InSequence seq; // Ensures that the expectations are called in the specified order

   EXPECT_CALL(mymock, powerButton()).Times(1);
   EXPECT_CALL(mymock, powerSignalProcessing()).Times(1);
   EXPECT_CALL(mymock, mainControlKnob()).Times(1);
#ifdef CONFIG_AUTO_OFF
   EXPECT_CALL(mymock, autoOff()).Times(1);
#endif
#if defined(CONFIG_BRIGHTNESS_ADJUSTMENT_ENABLED) && CONFIG_BRIGHTNESS_ADJUSTMENT_ENABLED == 1
   EXPECT_CALL(mymock, brightnessController()).Times(1);
#endif
   EXPECT_CALL(mymock, lightController()).Times(1);
   EXPECT_CALL(mymock, consoleInterface()).Times(1);

   spled();
}
