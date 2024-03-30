#pragma once
#ifndef __NAIVE_SPRITE_2D__
#define __NAIVE_SPRITE_2D__

#include <string>
#include "ogl_utils.hpp"

class game_window;

/**
 * 表示一个2D图像素材的不可变类。
 * 我们默认一个素材指的是一个固定图像上固定的一部分，所以它的uv是固定的。
 * 此类使用了一个简单的缓存，uv变换矩阵只计算一次。
 */
class sprite2d {
    using position = glut::position;
    std::string file_path;
    position uvpos;
    mutable bool _tex_mat_known = false;
    mutable glm::mat4 _tex_mat;
public:
    sprite2d(std::string_view path, const position& uv);
    ~sprite2d();
    const std::string& path() const { return file_path; }
    const position& uv() const { return uvpos; }
    const glm::mat4& tex_mat(unsigned int width, unsigned int height) const;
};

#endif // __NAIVE_SPRITE_2D__
