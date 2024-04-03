#pragma once
#ifndef __OGL_UTILS__
#define __OGL_UTILS__

#include "ogl_deps.hpp"

namespace glut {

struct position {
    int left, right, top, bottom;
    int width() const { return right - left; }
    int height() const { return bottom - top; }
};

inline const auto eye4 = glm::mat4(1.0f);

struct vertex_obj {
    gl::VertexArray vao;
    gl::ArrayBuffer vbo;
    template <typename FuncT>
    void with_buf_do(FuncT&& action) {
        gl::Bind(vao); gl::Bind(vbo);
        action(*this);
        gl::Unbind(vbo); gl::Unbind(vao);
    }
    template <typename FuncT>
    void with_obj_do(FuncT&& action) {
        gl::Bind(vao);
        action(*this);
        gl::Unbind(vao);
    }
};

// 以某点为中心旋转一定角度的变换矩阵
inline glm::mat4 centered_rotate2d(const glm::mat4& m, glm::f32 angle, glm::f32 cx, glm::f32 cy) {
    auto rot = glm::translate(m, glm::vec3(cx, cy, 0.0f));
    rot = glm::rotate(rot, angle, glm::vec3(0.0f, 0.0f, 1.0f));
    rot = glm::translate(rot, glm::vec3(-cx, -cy, 0.0f));
    return rot;
}

// 给定xy位置，算出素材的坐标变换矩阵
inline glm::mat4 xy_trans(position xy, unsigned int width, unsigned int height, const glm::mat4& transform) {
    auto m = glm::ortho<float>(0.0f, width, height, 0.0f) * transform;
    m = glm::translate(m, glm::vec3(xy.left, xy.top, 0.0f));
    m = glm::scale(m, glm::vec3(xy.width(), xy.height(), 1.0f));
    return m;
}

// 给定uv位置，算出素材的坐标变换矩阵
inline glm::mat4 uv_trans(position uv, unsigned int width, unsigned int height, const glm::mat4& transform) {
    // 事实上uv位置和xy位置算法一样
    // 但是xy位置的最终范围是[-1, 1]，而uv位置的最终范围为[0, 1]
    // 因此需要乘上这两个矩阵调整
    auto m = glm::scale(eye4, glm::vec3(0.5f, 0.5f, 1.0f));
    m = glm::translate(m, glm::vec3(1.0f, 1.0f, 0.0f));
    return m * xy_trans(uv, width, height, transform);
}

} // namespace glut

#endif // __OGL_UTILS__
