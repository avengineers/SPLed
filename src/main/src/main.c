#include "autoconf.h"

#if CONFIG_DUMMY_INTERFACE_DEFINED
#include "component.h"
extern int dummyInterface(void);
#endif

int main(void) {
#if (CONFIG_DUMMY_INTERFACE_DEFINED)
    return dummyInterface();
#else
    return 0;
#endif
}
