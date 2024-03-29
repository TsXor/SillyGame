#pragma once
#ifndef __NAIVE_SPRITE_2D__
#define __NAIVE_SPRITE_2D__

#include <string>
#include "ogl_deps.hpp"

class game_window;

/**
 * 表示一个2D图像素材的不可变类。
 */
class sprite2d {
public:
    struct position {
        int left, right, top, bottom;
        int width() const { return right - left; }
        int height() const { return bottom - top; }
    };
private:
    mutable bool _tex_mat_known = false;
    mutable glm::mat4 _tex_mat;
public:
    const std::string file_path;
    const position uv;
    sprite2d(std::string_view path, const position& uv);
    ~sprite2d();
    const glm::mat4& tex_mat(int width, int height) const;
};

#endif // __NAIVE_SPRITE_2D__
