#pragma once
#ifndef __SFOPT_LOGGER__
#define __SFOPT_LOGGER__

#include "silly_framework/config.h"

namespace silly_framework::logger {

SF_EXPORT size_t lib_identity();
SF_EXPORT const std::string& default_name();

// 获取logger
SF_EXPORT const std::string& name();
// 设置logger
SF_EXPORT void make(const std::string& logger_name, const std::string& filename);

} // namespace silly_framework::logger

#endif // __SFOPT_LOGGER__
