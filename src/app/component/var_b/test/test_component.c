#include <unity.h>

#include <component.h>

void setUp(void) {

}

void tearDown(void) {
    
}

void test_static_return_code(void)
{
    TEST_ASSERT_EQUAL_INT(1, dummyInterface());
}

void test_dummyInterface2_returns_100(void)
{
    TEST_ASSERT_EQUAL_INT(100, dummyInterface2());
}

int main(void)
{
    UNITY_BEGIN();

    RUN_TEST(test_static_return_code);
    RUN_TEST(test_dummyInterface2_returns_100);

    return UNITY_END();
}