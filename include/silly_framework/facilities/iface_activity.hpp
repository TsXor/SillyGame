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
    virtual bool vkey_only() { return true; }
    // 仅在键盘状态真正改变时触发
    virtual void on_key_change() {}
    // 在有键盘事件时触发，例如按住某键时会持续触发
    virtual void on_key_signal() {}
    virtual void on_tick(double this_time, double last_time) {}
};

} // namespace silly_framework

#endif // __SF_IFACE_ACTIVITY__
