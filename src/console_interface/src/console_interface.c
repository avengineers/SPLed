#include "console_interface.h"

#include "rte.h"
#include <stdio.h>
#include <windows.h>

static RGBColor previousLightValue = { 255, 255, 255 };

void consoleInterface(void) {
   RGBColor lightValue;

   RteGetLightValue(&lightValue);

   // Check if the light value has changed
   if (lightValue.red != previousLightValue.red ||
      lightValue.green != previousLightValue.green ||
      lightValue.blue != previousLightValue.blue) {

      // Update the previous light value
      previousLightValue = lightValue;

      //printf("%d\n", previousLightValue.blue);

      // Get the handle to the current output buffer ...
      HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

      // and hide the cursor ...
      CONSOLE_CURSOR_INFO cursorInfo;
      GetConsoleCursorInfo(hConsole, &cursorInfo);
      cursorInfo.bVisible = FALSE;
      SetConsoleCursorInfo(hConsole, &cursorInfo);

      // and print the LED representation.
      printf("\x1b[48;2;%d;%d;%dm", lightValue.red, lightValue.green, lightValue.blue);
      printf("LED\r");
   }
}
