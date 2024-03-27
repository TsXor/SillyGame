#pragma once
#ifndef __DEMO__
#define __DEMO__

#include "utilities/ogl_deps.hpp"
#include <oglwrap/shapes/rectangle_shape.h>
#include "facilities/iface_activity.hpp"

struct game_window;

class show_square : public iface_activity {
private:
    game_window& window;
    // Defines a full screen rectangle (see oglwrap/shapes/rectangle_shape.h)
    gl::RectangleShape rectangle_shape;

public:
    show_square(game_window& window_);
    ~show_square();
    
    virtual void render();
};

#endif // __DEMO__
