/* polyspace MISRA-C3:1.1 [Justified:Low] "External header windows.h not modifiable, but number of macro definitions acceptable for current compiler" */
#include "keyboard_interface.h"

#ifdef _WIN32
#include <windows.h>
#else
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/select.h>
#include <stdio.h>
#endif

#ifdef _WIN32
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
#else
// Unix/Linux implementation
static struct termios old_termios;
static int termios_configured = 0;

static void configure_terminal(void)
{
    if (!termios_configured)
    {
        struct termios new_termios;

        // Get current terminal settings
        tcgetattr(STDIN_FILENO, &old_termios);
        new_termios = old_termios;

        // Disable canonical mode and echo
        new_termios.c_lflag &= ~(ICANON | ECHO);
        new_termios.c_cc[VMIN] = 0;
        new_termios.c_cc[VTIME] = 0;

        // Apply new settings
        tcsetattr(STDIN_FILENO, TCSANOW, &new_termios);

        // Set stdin to non-blocking
        fcntl(STDIN_FILENO, F_SETFL, O_NONBLOCK);

        termios_configured = 1;
    }
}

static void restore_terminal(void)
{
    if (termios_configured)
    {
        tcsetattr(STDIN_FILENO, TCSANOW, &old_termios);
        termios_configured = 0;
    }
}

bool_t KeyboardInterfaceIsKeyPressed(int32_t keyCode)
{
    configure_terminal();

    fd_set readfds;
    struct timeval timeout;

    FD_ZERO(&readfds);
    FD_SET(STDIN_FILENO, &readfds);

    timeout.tv_sec = 0;
    timeout.tv_usec = 0;

    int result = select(STDIN_FILENO + 1, &readfds, NULL, NULL, &timeout);

    if (result > 0 && FD_ISSET(STDIN_FILENO, &readfds))
    {
        char c;
        if (read(STDIN_FILENO, &c, 1) > 0)
        {
            switch (keyCode)
            {
                case 27: // VK_ESCAPE
                    return (c == 27) ? TRUE : FALSE;
                case 13: // VK_RETURN
                    return (c == '\n' || c == '\r') ? TRUE : FALSE;
                case 32: // VK_SPACE
                    return (c == ' ') ? TRUE : FALSE;
                default:
                    // For ASCII keys, do direct comparison
                    if (keyCode >= 'A' && keyCode <= 'Z')
                        return (c == keyCode || c == (keyCode + 32)) ? TRUE : FALSE;
                    return (c == keyCode) ? TRUE : FALSE;
            }
        }
    }

    return FALSE;
}

// Cleanup function for proper terminal restoration
void KeyboardInterfaceCleanup(void)
{
    restore_terminal();
}
#endif
