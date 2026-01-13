#ifndef ADC_H
#define ADC_H

#include <stdint.h>

typedef struct
{
    uint32_t channel;
    uint32_t reference_voltage;
} ADC_Config;

void ADC_Read(ADC_Config *config, uint32_t *result);

void RteSetSupplyVoltage(uint32_t voltage);

void ADC_Main(void);

#endif // ADC_H
