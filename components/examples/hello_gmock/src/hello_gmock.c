/**
 * @file hello_gmock.c
 */

#include "hello_gmock.h"

int32_t ProcessSensorValue(void)
{
    int32_t data = 0;
    data = ReadSensorValue() * 2;
    return data;
}

int32_t ProcessSensorStatus(void)
{
    int32_t data = 0;
    ReadSensorStatus(&data);
    data *= 2;
    return data;
}

int32_t ProcessSensorResult(int32_t *const data)
{
    return ReadSensorResult(data);
}

void InitSensorConfig(SensorConfig_t *const data)
{
    data->threshold = 42;
    data->unit = 'C';
}

void ProcessSensorConfig(SensorConfig_t *const data)
{
    ReadSensorConfig(data);
}

void ProcessSensorConfigArray(SensorConfig_t *const data)
{
    ReadSensorConfigArray(data);
}

void ProcessSensorOutput(int32_t data)
{
    WriteSensorValue(data * 2);
}

void ProcessSensorCommand(int32_t data)
{
    int32_t localData = data * 2;
    WriteSensorCommand(&localData);
}
