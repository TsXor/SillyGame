#include "activity_manager.hpp"
#include <unordered_map>


activity_manager::activity_manager() {}
activity_manager::~activity_manager() {
    sync_current_state(); // 析构当前排队的
    while (!stack.empty()) { pop(); }
    sync_current_state(); // 析构上一句产生的
}

void activity_manager::sync_current_state() {
    const std::lock_guard guard(op_lock);
    for (auto act : poped) { delete act; }
    poped.clear();
    cur_snapshot = stack.empty() ? nullptr : stack.back();
}
