#include <gtest/gtest.h>
using namespace testing;

extern "C" {
#include "spled.h"
}

#include "mockup_src_spled.h"

TEST(spled, test_interface_call_order)
{
   CREATE_MOCK(mymock);

   InSequence seq; // Ensures that the expectations are called in the specified order

   EXPECT_CALL(mymock, keyboardInterface()).Times(1);
   EXPECT_CALL(mymock, powerSignalProcessing()).Times(1);
   EXPECT_CALL(mymock, lightController()).Times(1);
   EXPECT_CALL(mymock, consoleInterface()).Times(1);

   spled();
}
