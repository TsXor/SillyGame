#pragma once
#ifndef __MAIN_FRAME_HPP__
#define __MAIN_FRAME_HPP__

#include <oglwrap/shapes/rectangle_shape.h>

class show_square {
private:
    // Defines a full screen rectangle (see oglwrap/shapes/rectangle_shape.h)
    gl::RectangleShape rectangle_shape;

public:
    show_square();
    ~show_square();
    
    void render();
};

#endif // __MAIN_FRAME_HPP__
