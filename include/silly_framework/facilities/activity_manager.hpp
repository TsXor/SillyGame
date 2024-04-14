#pragma once
#ifndef __SF_ACTIVITY_MANAGER__
#define __SF_ACTIVITY_MANAGER__

#include <vector>
#include <mutex>
#include "silly_framework/facilities/iface_activity.hpp"

namespace silly_framework {

class activity_manager {
    std::vector<iface_activity*> stack, poped;
    iface_activity* cur_snapshot = nullptr;
    std::mutex op_lock;

public:
    activity_manager();
    ~activity_manager();

    // 构造一个Activity并压栈。
    template <typename SceneT, typename... ArgTs>
    void emplace(ArgTs&&... args) {
        const std::lock_guard guard(op_lock);
        stack.push_back(new SceneT(std::forward<ArgTs>(args)...));
    }
    // 弹出栈顶的Activity，放入待析构列表。
    void pop() {
        const std::lock_guard guard(op_lock);
        poped.push_back(stack.back()); stack.pop_back();
    }
    // 构造一个Activity并切换到它。
    template <typename SceneT, typename... ArgTs>
    void switch_to(ArgTs&&... args) {
        const std::lock_guard guard(op_lock);
        pop(); emplace<SceneT>(std::forward<ArgTs>(args)...);
    }

    bool empty() { return cur_snapshot == nullptr; }
    iface_activity& current() { return *cur_snapshot; }

    template <typename FuncT>
    void with_lock_do(FuncT&& action) {
        const std::lock_guard guard(op_lock);
        action();
    }

    // 获取当前实时的栈顶活动并进行操作
    // 若活动栈为空，操作不会执行
    template <typename FuncT>
    void with_realtime_current_do(FuncT&& action) {
        iface_activity* cur;
        {
            const std::lock_guard guard(op_lock);
            if (stack.empty()) { return; }
            cur = stack.back();
        }
        action(*cur);
    }

    // 同步全部活动状态
    void sync_current_state();
};

} // namespace silly_framework

#endif // __SF_ACTIVITY_MANAGER__
