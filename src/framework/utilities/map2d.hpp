#pragma once
#ifndef __NAIVE_MAP_2D__
#define __NAIVE_MAP_2D__

#include <string>
#include "ogl_utils.hpp"

class game_window;

/**
 * 表示一个2D地图的不可变类。
 */
class map2d {
    using position = glut::position;
    std::string file_path;
public:
    map2d(std::string_view path);
    ~map2d();
    const std::string& path() const { return file_path; }
    // 用此地图的指定位置填充整个“虚拟屏幕”
    void render(game_window& wnd, const position& uv);
};

#endif // __NAIVE_MAP_2D__
