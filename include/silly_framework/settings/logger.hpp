#pragma once
#ifndef __SFOPT_LOGGER__
#define __SFOPT_LOGGER__

namespace silly_framework::logger {

size_t lib_identity();
const std::string& default_name();

// 获取logger
const std::string& name();
// 设置logger
void make(const std::string& logger_name, const std::string& filename);

} // namespace silly_framework::logger

#endif // __SFOPT_LOGGER__
