#pragma once
#ifndef __ACTIVITY_MANAGER__
#define __ACTIVITY_MANAGER__

#include <vector>
#include "iface_activity.hpp"
#include "base_manager.hpp"

class activity_manager : public base_manager {
    std::vector<iface_activity*> stack, delete_pending;
    static void loop_job(game_window& wnd);

public:
    activity_manager(game_window& parent);
    ~activity_manager();

    // 构造一个Activity并压栈。
    template <typename SceneT, typename... ArgTs>
    void emplace(ArgTs... args) {
        stack.push_back(new SceneT(wnd(), std::forward<ArgTs>(args)...));
    }
    // 弹出栈顶的Activity，它会在下一次主循环被真正删除。
    void pop() {
        delete_pending.push_back(stack.back()); stack.pop_back();
    }
    // 构造一个Activity并切换到它。
    template <typename SceneT, typename... ArgTs>
    void switch_to(ArgTs... args) {
        pop(); emplace<SceneT>(std::forward<ArgTs>(args)...);
    }

    bool empty() { return stack.empty(); }
    iface_activity& current() { return *stack.back(); }
    iface_activity* last() {
        auto stack_size = stack.size();
        return stack_size > 1 ? stack[stack_size - 2] : nullptr;
    }
};

#endif // __ACTIVITY_MANAGER__
