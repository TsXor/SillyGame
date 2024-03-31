#pragma once
#ifndef __DEMO__
#define __DEMO__

#include "utilities/ogl_deps.hpp"
#include "facilities/iface_activity.hpp"
#include "utilities/sprite2d_ref.hpp"

class demo : public base_activity {
    sprite2d_ref spr;
    volatile double posx, posy;

public:
    demo(game_window& window);
    ~demo();
    
    void render() override;
    void tick(double this_time, double last_time) override;
};

#endif // __DEMO__
