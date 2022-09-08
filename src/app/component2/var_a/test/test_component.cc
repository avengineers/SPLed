#include <gtest/gtest.h>
using namespace testing;

extern "C" {
#include "../src/component.h"
}

#include "mockup_src_app_component2_var_a.h"

TEST(component2, test_static_return_code)
{
    CREATE_MOCK(mymock);

    EXPECT_EQ(2, dummyInterfaceC2());
}
