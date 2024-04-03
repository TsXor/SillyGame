#pragma once
#ifndef __ACTIVITY_MANAGER__
#define __ACTIVITY_MANAGER__

#include <vector>
#include "iface_activity.hpp"

class activity_manager {
    std::vector<iface_activity*> stack, poped;

public:
    activity_manager();
    ~activity_manager();

    // 构造一个Activity并压栈。
    template <typename SceneT, typename... ArgTs>
    void emplace(ArgTs&&... args) {
        stack.push_back(new SceneT(std::forward<ArgTs>(args)...));
    }
    // 弹出栈顶的Activity，放入待析构列表。
    void pop() {
        poped.push_back(stack.back()); stack.pop_back();
    }
    // 构造一个Activity并切换到它。
    template <typename SceneT, typename... ArgTs>
    void switch_to(ArgTs&&... args) {
        pop(); emplace<SceneT>(std::forward<ArgTs>(args)...);
    }

    bool empty() { return stack.empty(); }
    iface_activity& current() { return *stack.back(); }
    iface_activity* last() {
        auto stack_size = stack.size();
        return stack_size > 1 ? stack[stack_size - 2] : nullptr;
    }

    // 析构所有被弹出后排队的活动
    void destroy_poped_activities();
};

#endif // __ACTIVITY_MANAGER__
