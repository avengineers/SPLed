#include "keyboard_interface.h"
#include <windows.h>

boolean isKeyPressed(int key) {
    if (GetAsyncKeyState(key)) {
        return TRUE;
    } else {
        return FALSE;
    }
}

void keyboardInterface(){
    
}