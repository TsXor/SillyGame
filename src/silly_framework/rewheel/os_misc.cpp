#include "silly_framework/rewheel/os_misc.hpp"
#include <uvpp.hpp>

#ifdef _WIN32
#include <windows.h>
#endif
#ifdef __linux__
#include <unistd.h>
#endif

using namespace silly_framework::os_misc;
namespace impl = silly_framework::os_misc;

void impl::disable_ime() {
#ifdef _WIN32
    // https://github.com/glfw/glfw/issues/2176
    ImmDisableIME(GetCurrentThreadId());
#endif
}

void impl::encoding_spell(int argc, char** argv) {
    uvpp::make_windows_encoding_happy(argc, argv);
}

int32_t impl::proc_id() {
#ifdef _WIN32
    return GetCurrentProcessId();
#endif
#ifdef __linux__
    return getpid();
#endif
}
