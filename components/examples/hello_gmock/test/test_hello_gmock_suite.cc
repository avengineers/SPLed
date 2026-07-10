/**
 * @file
 */

#include <gtest/gtest.h>
using namespace testing;

extern "C"
{
#include "hello_gmock.h"
}

#include "mockup_components_examples_hello_gmock.h"

/**
 * @rst
 * .. test:: hello_gmock_suite.get_by_value
 *    :id: TS_COMP_A_010
 *    :tests: SWDD_COMP_A-010
 * @endrst
 */
TEST(hello_gmock_suite, get_by_value)
{
    /* Arrange */
    CREATE_MOCK(mymock);
    EXPECT_CALL(mymock, ReadSensorValue())
        .WillOnce(Return(13));

    /* Act and Assert */
    ASSERT_EQ(26, ProcessSensorValue());
}

/**
 * @rst
 * .. test:: hello_gmock_suite.get_by_pointer
 *    :id: TS_COMP_A_020
 *    :tests: SWDD_COMP_A-020
 * @endrst
 */
TEST(hello_gmock_suite, get_by_pointer)
{
    /* Arrange */
    CREATE_MOCK(mymock);
    EXPECT_CALL(mymock, ReadSensorStatus(_))
        .WillOnce(SetArgPointee<0>(42));

    /* Act and Assert */
    ASSERT_EQ(84, ProcessSensorStatus());
}

/**
 * @rst
 * .. test:: hello_gmock_suite.get_by_pointer_and_return_value
 *    :id: TS_COMP_A_030
 *    :tests: SWDD_COMP_A-030
 * @endrst
 */
TEST(hello_gmock_suite, get_by_pointer_and_return_value)
{
    /* Arrange */
    int value = 0;
    CREATE_MOCK(mymock);
    // Set up the mock to return a value and set the pointed data
    EXPECT_CALL(mymock, ReadSensorResult(_))
        .WillOnce(
            DoAll(
                SetArgPointee<0>(42),
                Return(2)));

    /* Act and Assert */
    ASSERT_EQ(2, ProcessSensorResult(&value));
    ASSERT_EQ(42, value);
}

/**
 * @rst
 * .. test:: hello_gmock_suite.init_data_structure
 *    :id: TS_COMP_A_035
 *    :tests: SWDD_COMP_A-035
 * @endrst
 */
TEST(hello_gmock_suite, init_data_structure)
{
    /* Arrange */
    SensorConfig_t data = {0, 0};

    /* Act */
    InitSensorConfig(&data);

    /* Assert */
    ASSERT_EQ(data.threshold, 42);
    ASSERT_EQ(data.unit, 'C');
}

MATCHER_P(EqualToSensorConfig, expected, "")
{
    return (arg.threshold == expected.threshold) && (arg.unit == expected.unit);
}

/**
 * @rst
 * .. test:: hello_gmock_suite.init_data_structure_with_matcher
 *    :id: TS_COMP_A_036
 *    :tests: SWDD_COMP_A-035
 * @endrst
 */
TEST(hello_gmock_suite, init_data_structure_with_matcher)
{
    /* Arrange */
    SensorConfig_t data = {0, 0};
    SensorConfig_t expected_data = {42, 'C'};

    /* Act */
    InitSensorConfig(&data);

    /* Assert */
    ASSERT_THAT(data, EqualToSensorConfig(expected_data));
}

/**
 * @rst
 * .. test:: hello_gmock_suite.get_data_structure_by_pointer
 *    :id: TS_COMP_A_040
 *    :tests: SWDD_COMP_A-040
 * @endrst
 */
TEST(hello_gmock_suite, get_data_structure_by_pointer)
{
    /* Arrange */
    SensorConfig_t result = {0, 0};
    SensorConfig_t input = {123, 42};
    CREATE_MOCK(mymock);
    // Set up the mock to fill the data structure
    EXPECT_CALL(mymock, ReadSensorConfig(_))
        .WillOnce(SetArgPointee<0>(input));

    /* Act */
    ProcessSensorConfig(&result);

    /* Assert */
    ASSERT_EQ(result.threshold, 123);
    ASSERT_EQ(result.unit, 42);
}

/**
 * @rst
 * .. test:: hello_gmock_suite.get_data_structure_by_pointer_1
 *    :id: TS_COMP_A_041
 *    :tests: SWDD_COMP_A-040
 * @endrst
 */
TEST(hello_gmock_suite, get_data_structure_by_pointer_1)
{
    /* Arrange */
    SensorConfig_t result = {0, 0};
    CREATE_MOCK(mymock);
    // Set up the mock to fill the struct fields using a lambda
    // This is an alternative to SetArgPointee that allows more complex logic
    // to be executed when the mock is called.
    EXPECT_CALL(mymock, ReadSensorConfig(_))
        .WillOnce(Invoke([](SensorConfig_t *data)
                         {
            data->threshold = 100+23;
            data->unit = 21*2; }));

    /* Act */
    ProcessSensorConfig(&result);

    /* Assert */
    ASSERT_EQ(result.threshold, 123);
    ASSERT_EQ(result.unit, 42);
}

/**
 * @rst
 * .. test:: hello_gmock_suite.get_data_structure_array
 *    :id: TS_COMP_A_050
 *    :tests: SWDD_COMP_A-050
 * @endrst
 */
TEST(hello_gmock_suite, get_data_structure_array)
{
    /* Arrange */
    SensorConfig_t result[2] = {{0, 0}, {0, 0}};
    SensorConfig_t input[2] = {{123, 'a'}, {456, 'b'}};
    CREATE_MOCK(mymock);
    // Set up the mock to fill the data structure array
    EXPECT_CALL(mymock, ReadSensorConfigArray(_))
        .WillOnce(SetArrayArgument<0>(input, input + 2));

    /* Act */
    ProcessSensorConfigArray(result);

    /* Assert */
    ASSERT_EQ(result[0].threshold, 123);
    ASSERT_EQ(result[0].unit, 'a');
    ASSERT_EQ(result[1].threshold, 456);
    ASSERT_EQ(result[1].unit, 'b');
}

/**
 * @rst
 * .. test:: hello_gmock_suite.set_by_value
 *    :id: TS_COMP_A_110
 *    :tests: SWDD_COMP_A-110
 * @endrst
 */
TEST(hello_gmock_suite, set_by_value)
{
    /* Arrange */
    CREATE_MOCK(mymock);
    EXPECT_CALL(mymock, WriteSensorValue(84))
        .Times(1);

    /* Act and Assert */
    ProcessSensorOutput(42);
}

/**
 * @rst
 * .. test:: hello_gmock_suite.set_data_by_pointer
 *    :id: TS_COMP_A_120
 *    :tests: SWDD_COMP_A-120
 * @endrst
 */
TEST(hello_gmock_suite, set_data_by_pointer)
{
    /* Arrange */
    CREATE_MOCK(mymock);
    EXPECT_CALL(mymock, WriteSensorCommand(Pointee(84)))
        .Times(1);

    /* Act */
    ProcessSensorCommand(42);
}
