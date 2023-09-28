#include "light_controller.h"

#include "rte.h"

void lightController(void) {
   if (RteGetPowerState() == POWER_STATE_OFF) {
      RGBColor color = {
         .red = 0,
         .green = 0,
         .blue = 0
      };
      RteSetLightValue(color);
   }
   else {
      RGBColor color = {
         .red = 0,
         .green = 128,
         .blue = 55
      };
      RteSetLightValue(color);
   }
}
