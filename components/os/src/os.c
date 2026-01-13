#include "os.h"
#include "autoconf.h"
#include "spled.h"
#include <unistd.h>
#include <time.h> // For nanosleep and struct timespec

static void OsTask(void)
{
    spled();
    // Delay in milliseconds using nanosleep
    struct timespec req = {0}, rem = {0};
    req.tv_sec = CONFIG_OS_TASK_PERIOD / 1000;
    req.tv_nsec = (CONFIG_OS_TASK_PERIOD % 1000) * 1000000L;
    while (nanosleep(&req, &rem) == -1) /* polyspace CERT-C:DCL39-C [Justified:Low] "All countermeasures implemented to avoid any unwanted structure padding." */
    {
        req = rem; // Retry if interrupted by signal
    }
}

void OsRun(void)
{
    while (1)
    {
        OsTask();
    }
}
