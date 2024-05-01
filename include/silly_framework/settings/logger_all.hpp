#pragma once
#ifndef __SFOPT_LOGGER_ALL__
#define __SFOPT_LOGGER_ALL__

#include <spdlog/spdlog.h>
#include "./logger.hpp"

namespace silly_framework::logger {

// 获取logger
SF_EXPORT std::shared_ptr<spdlog::logger> is();
// 设置logger
SF_EXPORT void is(std::shared_ptr<spdlog::logger> logger);

} // namespace silly_framework::logger

#endif // __SFOPT_LOGGER_ALL__
