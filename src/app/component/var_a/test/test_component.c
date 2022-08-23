#include <unity.h>

#include <component.h>
#include "mock_some_func.h"

void setUp(void) {

}

void tearDown(void) {
    
}

void test_dummyInterface(void)
{
    some_func_ExpectAndReturn(1, 2);
    TEST_ASSERT_EQUAL_INT(2, dummyInterface());
}

void test_anotherDummyInterface(void)
{
    TEST_ASSERT_EQUAL_INT(3, anotherDummyInterface(1));
    TEST_ASSERT_EQUAL_INT(3, anotherDummyInterface(2));
    TEST_ASSERT_EQUAL_INT(3, anotherDummyInterface(3));
    TEST_ASSERT_EQUAL_INT(3, anotherDummyInterface(4));
    TEST_ASSERT_EQUAL_INT(3, anotherDummyInterface(5));

    some_func_ExpectAndReturn(6, 7);
    TEST_ASSERT_EQUAL_INT(7, anotherDummyInterface(6));

    
    some_func_ExpectAndReturn(7, 8);
    TEST_ASSERT_EQUAL_INT(8, anotherDummyInterface(7));
}

int main(void)
{
    UNITY_BEGIN();

    RUN_TEST(test_dummyInterface);
    RUN_TEST(test_anotherDummyInterface);

    return UNITY_END();
}