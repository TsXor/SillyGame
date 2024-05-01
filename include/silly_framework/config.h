#pragma once
#ifndef __SF_CONFIG__
#define __SF_CONFIG__

#if defined(_WIN32)
    // MinGW可以识别__declspec
#   if defined(SF_BUILD)
#       define SF_EXPORT   __declspec(dllexport)
#   else
#       define SF_EXPORT   __declspec(dllimport)
#   endif
#elif defined(__GNUC__)
    // 编译选项需加-fvisibility=hidden
#   if defined(SF_BUILD)
#       define SF_EXPORT __attribute__ ((visibility ("default")))
#   else
#       define SF_EXPORT extern
#   endif
#else
#   define SF_EXPORT extern
#endif

#define GLAD_GLAPI_EXPORT
#ifdef SF_BUILD
#   define GLAD_GLAPI_EXPORT_BUILD
#endif

#endif // __SF_CONFIG__
