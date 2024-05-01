#pragma once
#ifndef __SFLIB_OGL_UTILS__
#define __SFLIB_OGL_UTILS__

#include <optional>
#include "silly_framework/ogl/oglwrap.hpp"

namespace silly_framework::glut {

struct coord { int x; int y; };

struct position {
    int left, right, top, bottom;
    
    int width() const { return right - left; }
    int height() const { return bottom - top; }
    static inline std::optional<position> intersect(const position& pos1, const position& pos2);
    auto intersect(const position& other) const { return intersect(*this, other); }
    coord center() const { return {(left + right) / 2, (top + bottom) / 2}; }
    
    position& operator+=(const coord& off) {
        left += off.x; right += off.x; top += off.y; bottom += off.y;
        return *this;
    }
    position& operator-=(const coord& off) {
        left -= off.x; right -= off.x; top -= off.y; bottom -= off.y;
        return *this;
    }
};

std::optional<position> position::intersect(const position& pos1, const position& pos2){
    position result{
        std::max(pos1.left, pos2.left), std::min(pos1.right, pos2.right),
        std::max(pos1.top, pos2.top), std::min(pos1.bottom, pos2.bottom)
    };
    bool valid = result.left < result.right && result.top < result.bottom;
    if (valid) { return result; } else { return std::nullopt; }
}

static inline position operator+(const position& pos, const coord& off) {
    return {pos.left + off.x, pos.right + off.x, pos.top + off.y, pos.bottom + off.y};
}
static inline position operator-(const position& pos, const coord& off) {
    return {pos.left - off.x, pos.right - off.x, pos.top - off.y, pos.bottom - off.y};
}
static inline coord operator+(const coord& off1, const coord& off2) {
    return {off1.x + off2.x, off1.y + off2.y};
}
static inline coord operator-(const coord& off1, const coord& off2) {
    return {off1.x - off2.x, off1.y - off2.y};
}


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

} // namespace silly_framework::glut

#endif // __SFLIB_OGL_UTILS__
