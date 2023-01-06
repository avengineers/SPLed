#include "autoconf.h"
#if CONFIG_USE_COMPONENT_1
#include "component-1.h"
#endif

int main(void){
    
    #if CONFIG_USE_COMPONENT_1
    component1_run();
    #endif
    return 0;
}