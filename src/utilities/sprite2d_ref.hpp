#pragma once
#ifndef __NAIVE_SPRITE_2D_REF__
#define __NAIVE_SPRITE_2D_REF__

#include "sprite2d.hpp"

class game_window;

struct sprite2d_ref {
    using position = sprite2d::position;
    game_window& wnd;
    const sprite2d& spr;
    sprite2d_ref(game_window& wnd_, const sprite2d& spr_);
    void blit(const position& xy, const glm::mat4& transform);
    void blit(const position& xy);
};

#endif // __NAIVE_SPRITE_2D_REF__
