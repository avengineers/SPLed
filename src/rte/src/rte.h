#ifndef __rte_h__
#define __rte_h__

// Define type for boolean
typedef unsigned char boolean;
#define TRUE 1
#define FALSE 0

// Define the PowerState type
typedef enum {
    POWER_STATE_OFF = 0,
    POWER_STATE_ON = 1
} PowerState;

void RteSetPowerState(PowerState state);
PowerState RteGetPowerState(void);

boolean RteGetPowerKeyPressed();
void RteSetPowerKeyPressed(boolean value);

void RteSetLightValue(int value);
int RteGetLightValue(void);

#endif // __rte_h__
