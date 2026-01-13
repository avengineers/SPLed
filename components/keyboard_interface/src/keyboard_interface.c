/* polyspace MISRA-C3:1.1 [Justified:Low] "External header windows.h not modifiable, but number of macro definitions acceptable for current compiler" */
#include "keyboard_interface.h"

#include <windows.h>

bool_t KeyboardInterfaceIsKeyPressed(int32_t keyCode)
{
    bool_t result;
    if (((uint16_t)GetAsyncKeyState(keyCode) & 0x8000U) != (uint16_t)0U)
    {
        result = TRUE;
    }
    else
    {
        result = FALSE;
    }
    return result;
}
