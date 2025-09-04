#ifndef KEYBOARD_INTERFACE_H
#define KEYBOARD_INTERFACE_H

#include "platform_types.h"

bool_t KeyboardInterfaceIsKeyPressed(int32_t keyCode);

#ifndef _WIN32
// Unix/Linux specific cleanup function
void KeyboardInterfaceCleanup(void);
#endif

#endif /* KEYBOARD_INTERFACE_H */
