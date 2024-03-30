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
};

#endif // __NAIVE_MAP_2D__
