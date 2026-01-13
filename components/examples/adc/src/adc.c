#include "adc.h"

void ADC_Main(void)
{
    ADC_Config config;
    uint32_t result;

    config.channel = 0;
    config.reference_voltage = 3300; // 3.3V in millivolts

    ADC_Read(&config, &result);

    RteSetSupplyVoltage(result);
}
