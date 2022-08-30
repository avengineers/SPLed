#include <gtest/gtest.h>
using namespace testing;

extern "C" {
#include "../src/component.h"
}

#include "mockup.h"

TEST(component2, test_static_return_code)
{
    CREATE_MOCK(mymock);

    EXPECT_EQ(2, dummyInterfaceC2());
}
