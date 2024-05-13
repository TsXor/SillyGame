#pragma once
#ifndef __ACTS_RENDER_UTILS__
#define __ACTS_RENDER_UTILS__

#include <silly_framework.hpp>

namespace acts {

namespace sf = silly_framework;

static inline void render_bg(sf::game_window& wnd, const std::string& path) {
    auto img = wnd.texman.get_texture(path);
    if (img != nullptr) { wnd.renman.blit(img->tex, sf::glut::full_xy, sf::glut::full_uv); }
}

static inline void render_img(sf::game_window& wnd, const std::string& path, const sf::glut::position& pos) {
    auto&& [vs_w, vs_h] = wnd.renman.vs_size();
    auto img = wnd.texman.get_texture(path);
    if (img != nullptr) { wnd.renman.blit(img->tex, sf::glut::xy_trans(pos, vs_w, vs_h, sf::glut::eye4), sf::glut::full_uv); }
}

static inline void render_number(sf::game_window& wnd, ssize_t n, const sf::glut::position& pos) {
    auto ns = std::to_string(n);
    auto rpos = pos;
    for (auto&& nc : ns) {
        render_img(wnd, std::format("power_numbers/{}.png", nc), rpos);
        rpos += sf::glut::coord(pos.width(), 0);
    }
};

} // namespace acts

#endif // __ACTS_RENDER_UTILS__
