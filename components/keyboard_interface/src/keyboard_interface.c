#include <windows.h>

boolean KeyboardInterfaceIsKeyPressed(int key)
{
    return (GetAsyncKeyState(key) & 0x8000) != 0;
}
