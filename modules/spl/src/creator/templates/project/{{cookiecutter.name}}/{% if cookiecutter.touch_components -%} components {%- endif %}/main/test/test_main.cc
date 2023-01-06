#include <gtest/gtest.h>
using namespace testing;

#include "mockup_components_main.h"

TEST(component__main, test_main_interface)
{
    CREATE_MOCK(mymock);

    EXPECT_CALL(mymock, component1_run());
}
