#ifndef HELLO_GMOCK_H
#define HELLO_GMOCK_H

#include <stdint.h>

extern int32_t ReadSensorValue(void);
extern int32_t ProcessSensorValue(void);

extern void ReadSensorStatus(int32_t *const data);
extern int32_t ProcessSensorStatus(void);

extern int32_t ReadSensorResult(int32_t *const data);
extern int32_t ProcessSensorResult(int32_t *const data);

typedef struct
{
    int32_t threshold;
    char unit;
} SensorConfig_t;

extern void InitSensorConfig(SensorConfig_t *const data);
extern void ReadSensorConfig(SensorConfig_t *const data);
extern void ProcessSensorConfig(SensorConfig_t *const data);

extern void ReadSensorConfigArray(SensorConfig_t *const data);
extern void ProcessSensorConfigArray(SensorConfig_t *const data);

extern void WriteSensorValue(int32_t data);
extern void ProcessSensorOutput(int32_t data);

extern void WriteSensorCommand(int32_t *const data);
extern void ProcessSensorCommand(int32_t data);

#endif /* HELLO_GMOCK_H */
