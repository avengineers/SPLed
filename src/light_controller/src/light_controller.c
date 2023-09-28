#include "light_controller.h"

#include "rte.h"

void lightController(void) {
   RGBColor color = {
      .red = 0,
      .green = 128,
      .blue = 55
   };
   RteSetLightValue(color);
}
