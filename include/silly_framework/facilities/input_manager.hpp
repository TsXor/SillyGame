#pragma once
#ifndef __SF_INPUT_MANAGER__
#define __SF_INPUT_MANAGER__

#include <vector>
#include <mutex>
#include <unordered_map>
#include "silly_framework/rewheel/ogl_env.hpp"
#include "silly_framework/facilities/vkey_def.hpp"
#include "silly_framework/facilities/iface_activity.hpp"

namespace silly_framework {

class input_manager {
    std::unordered_multimap<int, vkey::code> kc_r2v_map;
    volatile bool key_states[1024];
    std::recursive_mutex ks_lock;
public:
    // 键位表，修改后需刷新生效
    std::vector<int> key_map;
    
    input_manager();
    ~input_manager();

    // 刷新键位映射
    void flush_key_map();
    // 更新键位状态
    bool update_key_state(int key, int scancode, int action, int mods);
    
    template <typename FuncT>
    void with_key_states(FuncT&& handler) {
        const std::lock_guard guard(ks_lock);
        handler();
    }

    bool has_vkey(int rkc) { return kc_r2v_map.contains(rkc); }
    int key_code(vkey::code vkc) { return key_map[static_cast<size_t>(vkc)]; }
    bool pressed(int rkc) { return key_states[rkc]; }
    bool pressed(vkey::code vkc) { return key_states[key_code(vkc)]; }
};

} // namespace silly_framework

#endif // __SF_INPUT_MANAGER__
