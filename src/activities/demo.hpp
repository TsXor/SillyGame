#pragma once
#ifndef __DEMO__
#define __DEMO__

#include "utilities/ogl_deps.hpp"
#include "utilities/base_activity.hpp"
#include "utilities/sprite2d.hpp"

class demo : public base_activity {
    const sprite2d& spr;
    volatile double posx, posy;

public:
    demo(game_window& window);
    ~demo();
    
    void render() override;
    void tick(double this_time, double last_time) override;
};

#endif // __DEMO__
