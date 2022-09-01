#include <gtest/gtest.h>
using namespace testing;

extern "C" {
#include "component.h"
}

#include "mockup.h"

TEST(component__var_a, test_dummyInterface)
{
    CREATE_MOCK(mymock);

    EXPECT_CALL(mymock, some_func(1)).WillRepeatedly(Return(2));
    EXPECT_EQ(2, dummyInterface());
}

TEST(component__var_a, test_anotherDummyInterface)
{
    CREATE_MOCK(mymock);

    EXPECT_CALL(mymock, some_func(_)).WillRepeatedly(ReturnArg<0>());

    EXPECT_EQ(3, anotherDummyInterface(1));
    EXPECT_EQ(3, anotherDummyInterface(2));
    EXPECT_EQ(3, anotherDummyInterface(3));
    EXPECT_EQ(3, anotherDummyInterface(4));
    EXPECT_EQ(3, anotherDummyInterface(5));

    EXPECT_CALL(mymock, some_func(6)).WillRepeatedly(Return(7));
    EXPECT_EQ(7, anotherDummyInterface(6));

    EXPECT_CALL(mymock, some_func(7)).WillRepeatedly(Return(8));
    EXPECT_EQ(8, anotherDummyInterface(7));
}
