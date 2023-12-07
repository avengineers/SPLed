/**
 * @file
 */

#include <gtest/gtest.h>
using namespace testing;

extern "C" {
#include "component_a.h"
}

#include "mockup_src_examples_component_a.h"

/*!
* @rst
*
* .. test:: component_a.test_1
*    :id: TS_COMP_A-001
*    :tests: SWDD_COMP_A-001
*
* @endrst
*/
TEST(component_a, test_1)
{
    CREATE_MOCK(mymock);
    EXPECT_CALL(mymock, ReadSomeData(_)).WillOnce(SetArgPointee<0>(13));
    ASSERT_EQ(13, CheckReadSomeData());
}

/*!
* @rst
*
* .. test:: component_a.test_2
*    :id: TS_COMP_A-002
*    :tests: SWDD_COMP_A-002
*
* @endrst
*/
TEST(component_a, test_2)
{
    CREATE_MOCK(mymock);
    // Variable gets updated because return status is zero
    EXPECT_CALL(mymock, ReadSomeDataAndReturn(_)).WillOnce(DoAll(SetArgPointee<0>(13), Return(0)));
    ASSERT_EQ(13, CheckReadSomeDataAndReturn());
    // Variable does not get updated because the return status is non-zero
    EXPECT_CALL(mymock, ReadSomeDataAndReturn(_)).WillOnce(DoAll(SetArgPointee<0>(13), Return(1)));
    ASSERT_EQ(0, CheckReadSomeDataAndReturn());
}

/*!
* @rst
*
* .. test:: component_a.test_3
*    :id: TS_COMP_A-003
*    :tests: SWDD_COMP_A-003
*
* @endrst
*/
TEST(component_a, test_3)
{
    CREATE_MOCK(mymock);
    // Variables get updated because return status is zero
    EXPECT_CALL(mymock, ReadMultipleDataAndReturn(_, _)).WillOnce(
        DoAll(SetArgPointee<0>(5),
            SetArgPointee<1>(8),
            Return(0)
        ));
    ASSERT_EQ(13, CheckReadMultipleDataAndReturn());
    // Variables do not get updated because the return status is non-zero
    EXPECT_CALL(mymock, ReadMultipleDataAndReturn(_, _)).WillOnce(
        DoAll(SetArgPointee<0>(5),
            SetArgPointee<1>(8),
            Return(1)
        ));
    ASSERT_EQ(0, CheckReadMultipleDataAndReturn());
}

/*!
* @rst
*
* .. test:: component_a.test_4
*    :id: TS_COMP_A-004
*    :tests: SWDD_COMP_A-004
*
* @endrst
*/
TEST(component_a, test_4)
{
    CREATE_MOCK(mymock);

    MyDataType input = {
        .a = 13,
        .b = 'a'
    };

    EXPECT_CALL(mymock, ReadDataStructure(_)).WillOnce(SetArgPointee<0>(input));

    MyDataType result;
    CheckReadDataStructure(&result);

    ASSERT_EQ(13, result.a);
    ASSERT_EQ('a', result.b);
}

/*!
* @rst
*
* .. test:: component_a.test_5
*    :id: TS_COMP_A-005
*    :tests: SWDD_COMP_A-005
*
* @endrst
*/
TEST(component_a, test_5) {
    CREATE_MOCK(mymock);

    MyDataType input = {
        .a = 13,
        .b = 'a'
    };

    EXPECT_CALL(mymock, ReadDataStructure(AllOf(
        Field(&MyDataType::a, input.a),
        Field(&MyDataType::b, input.b)
    )));

    CheckReadDataStructure(&input);
}

/*!
* @rst
*
* .. test:: component_a.test_6
*    :id: TS_COMP_A-006
*    :tests: SWDD_COMP_A-006
*
* @endrst
*/
TEST(component_a, test_6)
{
    CREATE_MOCK(mymock);

    MyDataType input[MY_DATA_ARRAY_SIZE] = {
        {.a = 11, .b = 'a'},
        {.a = 22, .b = 'b'},
        {.a = 33, .b = 'c'},
    };

    EXPECT_CALL(mymock, ReadDataStructureArray(_)).WillOnce(SetArrayArgument<0>(input, input + MY_DATA_ARRAY_SIZE));

    MyDataType result[MY_DATA_ARRAY_SIZE];
    CheckReadDataStructureArray(result);

    ASSERT_EQ(11, result[0].a);
    ASSERT_EQ('a', result[0].b);
    ASSERT_EQ(22, result[1].a);
    ASSERT_EQ('b', result[1].b);
    ASSERT_EQ(33, result[2].a);
    ASSERT_EQ('c', result[2].b);
}

/*!
* @rst
*
* .. test:: component_a.test_7
*    :id: TS_COMP_A-007
*    :tests: SWDD_COMP_A-007
*
* @endrst
*/
TEST(component_a, test_7)
{
    CREATE_MOCK(mymock);

    MyDataType input[MY_DATA_ARRAY_SIZE] = {
        {.a = 11, .b = 'a'},
        {.a = 22, .b = 'b'},
        {.a = 33, .b = 'c'},
    };

    // Check that the ReadDataStructureArray input array has the same values as the one in input
    EXPECT_CALL(mymock, ReadDataStructureArray(_))
        .WillOnce(Invoke([&input](const MyDataType* array) {
        for (size_t i = 0; i < MY_DATA_ARRAY_SIZE; ++i) {
            EXPECT_EQ(input[i].a, array[i].a) << "Check 'a' for index " << i + 1;
            EXPECT_EQ(input[i].b, array[i].b) << "Check 'b' for index " << i + 1;
        }}));

    CheckReadDataStructureArray(input);

}
