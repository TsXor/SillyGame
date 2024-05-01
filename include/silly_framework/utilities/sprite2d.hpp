#pragma once
#ifndef __SFUT_SPRITE_2D__
#define __SFUT_SPRITE_2D__

#include <string>
#include "silly_framework/config.h"
#include "silly_framework/rewheel/ogl_utils.hpp"

namespace silly_framework {

class game_window;

/**
 * 表示一个2D图像素材的不可变类。
 * 我们默认一个素材指的是一个固定图像上固定的一部分，所以它的uv是固定的。
 * 此类使用了一个简单的缓存，uv变换矩阵只计算一次。
 */
class SF_EXPORT sprite2d {
protected:
    std::string file_path;
    glut::position uvpos;
    mutable std::optional<glm::mat4> cache_tex_mat;
public:
    sprite2d(std::string_view path, const glut::position& uv) : file_path(path), uvpos(uv) {}
    ~sprite2d() {}
    const std::string& path() const { return file_path; }
    const glut::position& uv() const { return uvpos; }
    // 计算纹理变换矩阵
    glm::mat4 tex_mat(unsigned int width, unsigned int height) const;
    // 将此素材渲染到“虚拟屏幕”上的指定位置，并对坐标施加线性变换
    void render(game_window& wnd, const glut::position& xy, const glm::mat4& transform = glut::eye4) const;
    void render(game_window& wnd, int cx, int cy, double scalev, const glm::mat4& transform = glut::eye4) const;
};

} // namespace silly_framework

#endif // __SFUT_SPRITE_2D__
