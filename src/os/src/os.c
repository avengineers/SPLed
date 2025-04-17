#include "autoconf.h"
#include "spled.h"
#include <unistd.h>

static void OsTask(void) {
    spled();
    // Delay in miliseconds
    usleep(CONFIG_OS_TASK_PERIOD * 1000);
}

void OsRun(void) {

    while (1)
    {
        OsTask();
    }
}
