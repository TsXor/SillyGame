#pragma once
#ifndef __SF_ACTIVITY_MANAGER__
#define __SF_ACTIVITY_MANAGER__

#include <vector>
#include <mutex>
#include <thread>
#include <atomic>
#include <functional>
#include "silly_framework/rewheel/circ_queue.hpp"
#include "silly_framework/facilities/iface_activity.hpp"

namespace silly_framework {

class activity_manager {
    using stack_type = std::vector<iface_activity*>;
    using stack_operator = std::function<void(stack_type&)>;
    stack_type stack;
    rewheel::concurrent_queue<stack_operator> event_queue;
    std::mutex render_lock;

public:
    activity_manager();
    ~activity_manager();

    // 构造一个Activity并压栈。
    template <typename SceneT, typename... ArgTs>
    void post_emplace(ArgTs&&... args) {
        auto act = new SceneT(std::forward<ArgTs>(args)...);
        event_queue.put_nowait(
            [act](stack_type& stack) { stack.push_back(act); }
        );
    }
    // 弹出栈顶的Activity。
    void post_pop() {
        event_queue.put_nowait(
            [](stack_type& stack) { delete stack.back(); stack.pop_back(); }
        );
    }
    // 构造一个Activity并切换到它。
    template <typename SceneT, typename... ArgTs>
    void post_switch(ArgTs&&... args) {
        auto act = new SceneT(std::forward<ArgTs>(args)...);
        event_queue.put_nowait(
            [act](stack_type& stack) { delete stack.back(); stack.back() = act; }
        );
    }
    // 强制清空活动栈，导致活动管理器终止。
    void post_stop() {
        post_nowait([](stack_type& stack){
            for (auto it = stack.rbegin(); it != stack.rend(); ++it) { delete *it; }
            stack.clear();
        });
    }

    template <typename... ArgTs>
    void post(ArgTs&&... args) { event_queue.put(std::forward<ArgTs>(args)...); }
    template <typename... ArgTs>
    void post_until(ArgTs&&... args) { event_queue.put_until(std::forward<ArgTs>(args)...); }
    template <typename... ArgTs>
    void post_timeout(ArgTs&&... args) { event_queue.put_timeout(std::forward<ArgTs>(args)...); }
    template <typename... ArgTs>
    void post_nowait(ArgTs&&... args) { event_queue.put_nowait(std::forward<ArgTs>(args)...); }

    void render();

    void loop(iface_activity& start);
};

} // namespace silly_framework

#endif // __SF_ACTIVITY_MANAGER__
