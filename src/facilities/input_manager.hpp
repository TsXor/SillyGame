#pragma once
#ifndef __INPUT_MANAGER__
#define __INPUT_MANAGER__

#include <vector>
#include <unordered_map>
#include "utilities/ogl_utils.hpp"
#include "base_manager.hpp"
#include "utilities/vkey_def.hpp"

class input_manager : public base_manager {
    std::unordered_multimap<int, vkey::code> kc_r2v_map;

public:
    // 键位表，修改后需刷新生效
    std::vector<int> key_map;
    
    input_manager(game_window& parent);
    ~input_manager();

    // 刷新键位映射
    void flush_key_map();
    // 处理键盘事件
    void on_key_event(int key, int action, int mods);
};

#endif // __INPUT_MANAGER__
