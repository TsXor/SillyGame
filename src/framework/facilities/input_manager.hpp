#pragma once
#ifndef __INPUT_MANAGER__
#define __INPUT_MANAGER__

#include <vector>
#include <unordered_map>
#include "framework/utilities/ogl_utils.hpp"
#include "framework/utilities/vkey_def.hpp"
#include "iface_activity.hpp"

class input_manager {
    std::unordered_multimap<int, vkey::code> kc_r2v_map;
    bool key_states[1024];
    std::mutex ks_lock;
public:
    // 键位表，修改后需刷新生效
    std::vector<int> key_map;
    
    input_manager();
    ~input_manager();

    // 刷新键位映射
    void flush_key_map();
    // 处理键盘事件
    void on_key_event(int key, int action, int mods, iface_activity& active);
    
    template <typename FuncT>
    void with_key_states(FuncT&& handler) {
        const std::lock_guard guard(ks_lock);
        handler();
    }

    inline int key_code(vkey::code vkc) { return key_map[static_cast<size_t>(vkc)]; }
    inline bool pressed(vkey::code vkc) { return key_states[key_code(vkc)]; }
};

#endif // __INPUT_MANAGER__
