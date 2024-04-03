#include "os_misc.hpp"

#ifdef _WIN32
#include <windows.h>
#pragma comment(lib, "imm32.lib")
#endif

#ifdef _WIN32
// https://github.com/glfw/glfw/issues/2176
void disable_ime() {
    ImmDisableIME(GetCurrentThreadId());
}
#else
void disable_ime() {}
#endif
