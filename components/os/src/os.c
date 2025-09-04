#include "os.h"
#include "autoconf.h"
#include "spled.h"

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#include <time.h>
#endif

static void OsTask(void)
{
    spled();
#ifdef _WIN32
    Sleep(CONFIG_OS_TASK_PERIOD);
#else
    // Delay in milliseconds using nanosleep
    struct timespec req = {0}, rem = {0};
    req.tv_sec = CONFIG_OS_TASK_PERIOD / 1000;
    req.tv_nsec = (CONFIG_OS_TASK_PERIOD % 1000) * 1000000L;
    while (nanosleep(&req, &rem) == -1) /* polyspace CERT-C:DCL39-C [Justified:Low] "All countermeasures implemented to avoid any unwanted structure padding." */
    {
        req = rem; // Retry if interrupted by signal
    }
#endif
}

void OsRun(void)
{
    while (1)
    {
        OsTask();
    }
}
