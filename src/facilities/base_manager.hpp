#pragma once
#ifndef __BASE_MANAGER__
#define __BASE_MANAGER__

#include <vector>
#include "iface_activity.hpp"

class game_window;

/**
 * 管理器基类。
 * 管理器是游戏窗口的成员，需要和游戏窗口本身和其他管理器交互。
 * 为了防止循环依赖，编写了这个类。
 */
class base_manager {
protected:
    game_window& parent;

public:
    using job_type = void(*)(game_window&);

    base_manager(game_window& parent_);
    ~base_manager();
    // 查询此管理器所属的窗口
    game_window& wnd();
    // 添加“主循环回调”，析构时自动释放
    void add_job(job_type fn);
};

#endif // __BASE_MANAGER__
