#include <gtest/gtest.h>
using namespace testing;

extern "C"
{
#include "autoconf.h"
#include "spled.h"
}

#include "mockup_test_spled_integration.h"

TEST(spled, test_interface_call_order)
{
   /* Arrange */
   RGBColor lightValue;
   CREATE_MOCK(mymock);

   EXPECT_CALL(mymock, KeyboardInterfaceIsKeyPressed(_)).WillRepeatedly(Return(true));
   EXPECT_CALL(mymock, consoleInterface()).Times(10);

   /* Act */
   for (int i = 0; i < 10; i++)
   {
      spled();
   }

   /* Assert */
   ASSERT_EQ(POWER_STATE_ON, RteGetPowerState());
   RteGetLightValue(&lightValue);
   ASSERT_EQ(lightValue.rgbGreenValue, 128);
}
