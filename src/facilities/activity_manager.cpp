#include <unordered_map>
#include "activity_manager.hpp"
#include "game_window.hpp"

activity_manager::activity_manager(game_window& parent):
    base_manager(parent) { add_job(loop_job); }
activity_manager::~activity_manager() {
    while (!stack.empty()) { pop(); }
    for (auto dp : delete_pending) { delete dp; }
}

void activity_manager::loop_job(game_window& wnd) {
    auto& delete_pending = wnd.actman.delete_pending;
    for (auto dp : delete_pending) { delete dp; }
    delete_pending.clear();
}
