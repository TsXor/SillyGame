#pragma once
#ifndef __IFACE_ACTIVITY__
#define __IFACE_ACTIVITY__

/**
 * Activity接口。不同类别的Activity需要继承并实现对应方法。
 * 注：学过安卓GUI的大概都知道为什么取这个名。
 */
struct iface_activity {
    virtual ~iface_activity() {};
    virtual void render() = 0;
    virtual void on_key(int key, int scancode, int action, int mods) {};
};

#endif // __IFACE_ACTIVITY__
