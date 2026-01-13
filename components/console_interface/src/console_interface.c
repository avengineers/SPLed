/* polyspace MISRA-C3:1.1 [Justified:Low] "External header windows.h not modifiable, but number of macro definitions acceptable for current compiler" */
#include "console_interface.h"

#include <stdio.h>
#include <windows.h>

void consoleInterface(void)
{
    RGBColor lightValue;
    static RGBColor previousLightValue = {255, 255, 255};

    RteGetLightValue(&lightValue);

    // Check if the light value has changed
    if ((lightValue.rgbRedValue != previousLightValue.rgbRedValue) ||
        (lightValue.rgbGreenValue != previousLightValue.rgbGreenValue) ||
        (lightValue.rgbBlueValue != previousLightValue.rgbBlueValue))
    {
        // Update the previous light value
        previousLightValue = lightValue;

        // Get the handle to the current output buffer ...
        const HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

        // and hide the cursor ...
        CONSOLE_CURSOR_INFO ledConsoleCursorInfo;
        if (GetConsoleCursorInfo(hConsole, &ledConsoleCursorInfo) != 0)
        {
            ledConsoleCursorInfo.bVisible = FALSE;
            (void)SetConsoleCursorInfo(hConsole, &ledConsoleCursorInfo);
        }

        // and print the LED representation.
        (void)printf("\x1b"
                     "[48;2;%d;%d;%dm",
                     lightValue.rgbRedValue, lightValue.rgbGreenValue, lightValue.rgbBlueValue);
        (void)printf("LED\r");
    }
}
