#include "activity_manager.hpp"
#include <unordered_map>


activity_manager::activity_manager() {}
activity_manager::~activity_manager() {
    destroy_poped_activities(); // 析构当前排队的
    while (!stack.empty()) { pop(); }
    destroy_poped_activities(); // 析构上一句产生的
}

void activity_manager::destroy_poped_activities() {
    for (auto act : poped) { delete act; }
    poped.clear();
}
