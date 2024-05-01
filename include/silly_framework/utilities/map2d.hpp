#pragma once
#ifndef __SFUT_MAP_2D__
#define __SFUT_MAP_2D__

#include <string>
#include <span>
#include <coutils.hpp>
#include "silly_framework/config.h"
#include "silly_framework/rewheel/ogl_utils.hpp"

namespace silly_framework {

class game_window;

/**
 * 表示一个2D地图的不可变类。
 * 一个地图图像由若干“地图块”构成，每个“地图块”可看作一个素材。
 * 此类使用了一个简单的缓存，uv变换矩阵只计算一次。
 */
class SF_EXPORT map2d {
public:
    using block_pos = std::pair<glut::position, glut::coord>;
protected:
    std::string file_path;
    std::vector<block_pos> pos_parts;
    mutable std::vector<glm::mat4> cache_tex_mats;
public:
    template <typename... ArgTs>
    map2d(std::string_view path, ArgTs&&... parts_args):
        file_path(path), pos_parts(std::forward<ArgTs>(parts_args)...) {}
    ~map2d() {}
    const std::string& path() const { return file_path; }
    std::span<const block_pos> parts() const { return pos_parts; }
    // 计算所有地图块的纹理变换矩阵
    coutils::generator<glm::mat4> tex_mats(unsigned int width, unsigned int height) const;
    // 用此地图的指定位置填充整个“虚拟屏幕”
    void render(game_window& wnd, const glut::position& uv) const;
    // 用此地图的指定位置填充整个“虚拟屏幕”
    void render(game_window& wnd, int cx, int cy, double scalev) const;
};

} // namespace silly_framework

#endif // __SFUT_MAP_2D__
