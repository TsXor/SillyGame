#pragma once
#ifndef __RENDER_MANAGER__
#define __RENDER_MANAGER__

#include <string>
#include <tuple>
#include "utilities/ogl_deps.hpp"
#include "base_manager.hpp"
#include "utilities/sprite2d.hpp"

extern const glm::mat4 eye_mat4;

class render_manager : public base_manager {
public:
    struct gl_vertex {
        gl::VertexArray vao;
        gl::ArrayBuffer vbo;
        template <typename FuncT>
        void with_buf_do(FuncT action) {
            gl::Bind(vao); gl::Bind(vbo);
            action(*this);
            gl::Unbind(vbo); gl::Unbind(vao);
        }
        template <typename FuncT>
        void with_obj_do(FuncT action) {
            gl::Bind(vao);
            action(*this);
            gl::Unbind(vao);
        }
    };

    using position = sprite2d::position;

private:
    // bilt用到的OpenGL资源
    gl_vertex blit_data;
    // “虚拟屏幕”的宽和高
    unsigned int vs_w = 0, vs_h = 0;

public:
    render_manager(game_window& parent);
    ~render_manager();

    std::tuple<unsigned int, unsigned int> vs_size() { return {vs_w, vs_h}; }
    void vs_size(unsigned int w, unsigned int h) { vs_w = w; vs_h = h; }

    // 使用指定路径的纹理，如果未加载完成，不使用任何纹理
    void use_texture(const std::string_view& path);
    // 使用占据窗口中央指定宽高比的视口
    void center_viewport(unsigned int hint_width, unsigned int hint_height);
    // 使用“虚拟屏幕”的视口
    void vscreen_viewport();
    // 使用占据整个窗口的视口
    void full_viewport();

    // 将指定纹理的一部分渲染到“虚拟屏幕”上的指定位置
    void blit(const sprite2d& spr, const position& xy, const glm::mat4& transform);
    void blit(const sprite2d& spr, const position& xy);
};

#endif // __RENDER_MANAGER__
