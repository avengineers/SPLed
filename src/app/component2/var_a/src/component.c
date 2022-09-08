#include "component.h"
#include "some_func_other.h"

volatile int x;

int dummyInterfaceC2(void) {
    some_other_folder_other_func(x, 2*x);
    return 2;
}
