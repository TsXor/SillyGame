#pragma once
#ifndef __IFACE_ACTIVITY__
#define __IFACE_ACTIVITY__

#include "utilities/vkey_def.hpp"

class game_window;

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

/**
 * Activity基类。大多数情况下，我们需要保存所属窗口的引用。
 */
struct base_activity : public iface_activity {
    game_window& parent;
    base_activity(game_window& wnd) : parent(wnd) {}
};

#endif // __IFACE_ACTIVITY__
