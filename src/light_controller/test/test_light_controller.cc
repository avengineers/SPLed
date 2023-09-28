#include <gtest/gtest.h>
using namespace testing;

extern "C" {
#include "light_controller.h"
}

#include "mockup_src_light_controller.h"

TEST(light_controller, test_interface_call_order)
{
   CREATE_MOCK(mymock);

   InSequence seq; // Ensures that the expectations are called in the specified order

   // TODO: replace _ with a concrete value (no idea how to define operators for C structs here.)
   EXPECT_CALL(mymock, RteSetLightValue(_)).Times(1);

   lightController();
}
