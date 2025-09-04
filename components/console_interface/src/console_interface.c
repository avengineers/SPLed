/* polyspace MISRA-C3:1.1 [Justified:Low] "External header windows.h not modifiable, but number of macro definitions acceptable for current compiler" */
#include "console_interface.h"

#include <stdio.h>

#ifdef _WIN32
#include <windows.h>
#else
#include <termios.h>
#include <unistd.h>
#endif

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

#ifdef _WIN32
        // Get the handle to the current output buffer ...
        const HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

        // and hide the cursor ...
        CONSOLE_CURSOR_INFO ledConsoleCursorInfo;
        if (GetConsoleCursorInfo(hConsole, &ledConsoleCursorInfo) != 0)
        {
            ledConsoleCursorInfo.bVisible = FALSE;
            (void)SetConsoleCursorInfo(hConsole, &ledConsoleCursorInfo);
        }
#else
        // Unix/Linux: Hide cursor using ANSI escape sequence
        (void)printf("\033[?25l");
#endif

        // Print the LED representation with ANSI color codes (works on both platforms)
        (void)printf("\x1b"
                     "[48;2;%d;%d;%dm",
                     lightValue.rgbRedValue, lightValue.rgbGreenValue, lightValue.rgbBlueValue);
        (void)printf("LED\r");

#ifndef _WIN32
        // Flush output buffer on Unix/Linux
        (void)fflush(stdout);
#endif
    }
}
