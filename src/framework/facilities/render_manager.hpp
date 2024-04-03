#pragma once
#ifndef __RENDER_MANAGER__
#define __RENDER_MANAGER__

#include <string>
#include <tuple>
#include <optional>
#include "framework/utilities/ogl_utils.hpp"
#include "framework/utilities/ogl_env.hpp"

class render_manager {
    using position = glut::position;
    // OpenGL窗口
    ogl_window& gl_wnd;
    // bilt用到的OpenGL资源，懒构造
    std::optional<glut::vertex_obj> _blit_data = std::nullopt;
    glut::vertex_obj& blit_data();
    // “虚拟屏幕”的宽和高
    unsigned int vs_w = 0, vs_h = 0;

public:
    render_manager(ogl_window& gl_wnd_);
    ~render_manager();

    std::tuple<unsigned int, unsigned int> vs_size() { return {vs_w, vs_h}; }
    void vs_size(unsigned int w, unsigned int h) { vs_w = w; vs_h = h; }

    // 使用指定路径的纹理，如果未加载完成，不使用任何纹理
    void use_texture(const std::string& path);
    // 使用占据窗口中央指定宽高比的视口
    void center_viewport(unsigned int hint_width, unsigned int hint_height);
    // 使用“虚拟屏幕”的视口
    void vscreen_viewport();
    // 使用占据整个窗口的视口
    void full_viewport();

    // 将指定纹理经xy与uv变换后渲染
    void blit(gl::Texture2D& tex, const glm::mat4& xy, const glm::mat4& uv);
};

#endif // __RENDER_MANAGER__
