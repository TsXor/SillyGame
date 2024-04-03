#pragma once
#ifndef __UTIL_MACROS__
#define __UTIL_MACROS__

#define SELF_BOUND_METHOD(fn_name) std::bind(std::remove_pointer_t<decltype(this)>::fn_name, this)
#define STATIC_ARRAY_SIZE(arr) (sizeof(arr) / sizeof(arr[0]))

#endif // __UTIL_MACROS__
