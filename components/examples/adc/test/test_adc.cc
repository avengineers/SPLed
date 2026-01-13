#include <gtest/gtest.h>
using namespace testing;

extern "C"
{
#include "adc.h"
}

#include "mockup_components_examples_adc.h"

TEST(adc, test_adc_reading)
{
    CREATE_MOCK(mymock);

    EXPECT_CALL(mymock, ADC_Read(_, _)).WillOnce(SetArgPointee<1>(2500));
    EXPECT_CALL(mymock, RteSetSupplyVoltage(2500));

    ADC_Main();
}
