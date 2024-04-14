#pragma once
#ifndef __SFLIB_OS_MISC__
#define __SFLIB_OS_MISC__

#include <cstdint>

namespace silly_framework::os_misc {

void disable_ime();
void encoding_spell(int argc, char** argv);
int32_t proc_id();

} // namespace silly_framework::os_misc

#endif // __SFLIB_OS_MISC__
