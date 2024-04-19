#include "silly_framework/facilities/activity_manager.hpp"
#include <unordered_map>

using namespace silly_framework;

activity_manager::activity_manager() : event_queue(1024) {}
activity_manager::~activity_manager() {}

void activity_manager::loop(iface_activity& start) {
    stack.push_back(&start);
    while (true) {
        auto op = event_queue.get();
        const std::lock_guard guard(render_lock);
        op(stack);
        if (stack.empty()) { break; }
    }
}

void activity_manager::render() {
    const std::lock_guard guard(render_lock);
    if (!stack.empty()) { stack.back()->render(); }
}
