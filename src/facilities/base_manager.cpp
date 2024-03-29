#include <unordered_map>
#include "base_manager.hpp"
#include "game_window.hpp"


static std::unordered_map<base_manager*, game_window::job_handle_type> job_storage;

base_manager::base_manager(game_window& parent_) : parent(parent_) {}
base_manager::~base_manager() {
    auto job_it = job_storage.find(this);
    if (job_it != job_storage.end()) {
        parent.remove_loop_job(job_it->second);
    }
}

game_window& base_manager::wnd() {
    return parent;
}

void base_manager::add_job(job_type fn) {
    job_storage[this] = parent.add_loop_job(fn);
}
