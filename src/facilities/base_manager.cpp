#include <unordered_map>
#include "base_manager.hpp"
#include "game_window.hpp"


static std::unordered_map<base_manager*, game_window*> parent_storage;
static std::unordered_map<base_manager*, game_window::job_handle_type> job_storage;

base_manager::base_manager(game_window& parent) {
    parent_storage[this] = &parent;
}
base_manager::~base_manager() {
    auto job_it = job_storage.find(this);
    auto parent_it = parent_storage.find(this);
    if (job_it != job_storage.end()) {
        parent_it->second->remove_loop_job(job_it->second);
    }
    parent_storage.erase(parent_it);
}

game_window& base_manager::wnd() {
    return *(parent_storage[this]);
}

void base_manager::add_job(job_type fn) {
    job_storage[this] = parent_storage[this]->add_loop_job(fn);
}
