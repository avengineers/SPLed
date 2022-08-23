#include <unity.h>

#include <component.h>

void setUp(void) {

}

void tearDown(void) {
    
}

void test_static_return_code(void)
{
    TEST_ASSERT_EQUAL_INT(2, dummyInterfaceC2());
}

int main(void)
{
    UNITY_BEGIN();

    RUN_TEST(test_static_return_code);

    return UNITY_END();
}