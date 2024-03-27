#pragma once
#ifndef __RENDER_MANAGER__
#define __RENDER_MANAGER__

#include "base_manager.hpp"

class render_manager : public base_manager {
public:
    render_manager(game_window& parent);
    ~render_manager();

    // 使用指定路径的纹理，如果未加载完成，不使用任何纹理
    void use_texture(const char* path);
    // 使用占据窗口中央指定宽高比的视口
    void center_viewport(int hint_width, int hint_height);
    // 使用占据整个窗口的视口
    void full_viewport();
};

#endif // __RENDER_MANAGER__
