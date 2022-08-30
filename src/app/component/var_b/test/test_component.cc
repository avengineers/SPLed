extern "C" {
#include "../src/component.h"
}

#include "mockup.h"

TEST(component__var_b, test_static_return_code)
{
    CREATE_MOCK(mymock);

    EXPECT_EQ(1, dummyInterface());
}

TEST(component__var_b, test_dummyInterface2_returns_100)
{
    EXPECT_EQ(100, dummyInterface2());
}
