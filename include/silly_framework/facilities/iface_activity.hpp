#pragma once
#ifndef __SF_IFACE_ACTIVITY__
#define __SF_IFACE_ACTIVITY__

#include "silly_framework/facilities/vkey_def.hpp"

namespace silly_framework {

/**
 * Activity接口。不同类别的Activity需要继承并实现对应方法。
 * 注：学过安卓GUI的大概都知道为什么取这个名。
 */
struct iface_activity {
    virtual ~iface_activity() {};
    virtual void render() = 0;
    virtual vkey::mode key_mode() { return vkey::mode::VKEY_ONLY; }
    virtual void on_key_event(vkey::code vkc, int rkc, int action, int mods) {}
    virtual void tick(double this_time, double last_time) {}
};

} // namespace silly_framework

#endif // __SF_IFACE_ACTIVITY__
