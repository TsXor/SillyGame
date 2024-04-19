#include "silly_framework/facilities/input_manager.hpp"

using namespace silly_framework;

static const std::vector<int> key_map_default({
    /* NO_VKEY */ 0,
    /* LEFT */ GLFW_KEY_J,
    /* RIGHT */ GLFW_KEY_L,
    /* UP */ GLFW_KEY_I,
    /* DOWN */ GLFW_KEY_K,
    /* CHECK */ GLFW_KEY_Z,
    /* MENU */ GLFW_KEY_X,
    /* SKIP */ GLFW_KEY_X,
    /* CANCEL */ GLFW_KEY_C,
});

input_manager::input_manager() : key_map(key_map_default) {
    flush_key_map();
    for (auto& ks : key_states) { ks = false; }
}
input_manager::~input_manager() {}

void input_manager::flush_key_map() {
    for (size_t vkc = 0; vkc < key_map.size(); ++vkc) {
        kc_r2v_map.emplace(key_map[vkc], static_cast<vkey::code>(vkc));
    }
}

bool input_manager::update_key_state(int key, int scancode, int action, int mods) {
    const std::lock_guard guard(ks_lock);
    auto may_update = [&](int key, bool new_state) -> bool {
        bool did_update = key_states[key] != new_state;
        key_states[key] = new_state;
        return did_update;
    };
    if (action == GLFW_PRESS) { return may_update(key, true); }
    if (action == GLFW_RELEASE) { return may_update(key, false); }
    return false;
}
