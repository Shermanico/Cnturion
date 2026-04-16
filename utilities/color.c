#include <color.h>

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

void initColors(void) {
#ifdef _WIN32
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hOut == INVALID_HANDLE_VALUE) {
        return;
    }

    DWORD dwMode = 0;
    if (!GetConsoleMode(hOut, &dwMode)) {
        return;
    }

    // ENABLE_VIRTUAL_TERMINAL_PROCESSING is 0x0004
    dwMode |= 0x0004;
    SetConsoleMode(hOut, dwMode);
#endif
}
