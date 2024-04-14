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

void input_manager::on_key_event(int key, int action, int mods, iface_activity& active) {
    {
        const std::lock_guard guard(ks_lock);
        if (action == GLFW_PRESS) { key_states[key] = true; }
        if (action == GLFW_RELEASE) { key_states[key] = false; }
    }
    switch (active.key_mode()) {
        case vkey::mode::RKEY_ONLY: {
            active.on_key_event(vkey::code::NO_VKEY, key, action, mods);
        } break;
        
        case vkey::mode::VKEY_ONLY: {
            for (auto it = kc_r2v_map.find(key); it != kc_r2v_map.end(); ++it) {
                active.on_key_event(it->second, key, action, mods);
            }
        } break;

        case vkey::mode::MIXED: {
            bool have_vkey = false;
            for (auto it = kc_r2v_map.find(key); it != kc_r2v_map.end(); ++it) {
                have_vkey = true;
                active.on_key_event(it->second, key, action, mods);
            }
            if (!have_vkey) {
                active.on_key_event(vkey::code::NO_VKEY, key, action, mods);
            }
        } break;
    }
}
