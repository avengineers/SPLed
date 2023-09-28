#include <gtest/gtest.h>
using namespace testing;

extern "C" {
#include "power_signal_processing.h"
}

#include "mockup_src_power_signal_processing.h"

TEST(power_signal_processing, test_power_stays_off)
{
   CREATE_MOCK(mymock);

   power_signal_processing();
}
