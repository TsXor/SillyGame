#pragma once
#ifndef __SFLIB_OS_MISC__
#define __SFLIB_OS_MISC__

#include <cstdint>
#include "silly_framework/config.h"

namespace silly_framework::os_misc {

SF_EXPORT void disable_ime();
SF_EXPORT void encoding_spell(int argc, char** argv);
SF_EXPORT int32_t proc_id();

} // namespace silly_framework::os_misc

#endif // __SFLIB_OS_MISC__
