#include "input_manager.hpp"
#include "game_window.hpp"


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

input_manager::input_manager(game_window& parent) : base_manager(parent),
key_map(key_map_default) { flush_key_map(); }

input_manager::~input_manager() {}

void input_manager::flush_key_map() {
    for (size_t vkc = 0; vkc < key_map.size(); ++vkc) {
        kc_r2v_map.emplace(key_map[vkc], static_cast<vkey::code>(vkc));
    }
}

void input_manager::on_key_event(int key, int action, int mods) {
    auto& active = parent.actman.current();
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
