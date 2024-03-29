#pragma once
#ifndef __DEMO__
#define __DEMO__

#include "utilities/ogl_deps.hpp"
#include "facilities/iface_activity.hpp"
#include "utilities/sprite2d_ref.hpp"

class show_square : public base_activity {
    sprite2d_ref spr;

public:
    show_square(game_window& window);
    ~show_square();
    
    virtual void render();
};

#endif // __DEMO__
