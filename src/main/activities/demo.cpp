#include "demo.hpp"
#include "main/static_sprites.hpp"


demo::demo(game_window& window) : base_activity(window),
spr(sprites::container_small()) {
    parent.renman.vs_size(1024, 768);
    auto&& [vs_w, vs_h] = parent.renman.vs_size();
    posx = vs_w / 2; posy = vs_h / 2;
}

demo::~demo() = default;

void demo::tick(double this_time, double last_time) {
    constexpr double velocity = 200;
    double dt = this_time - last_time;
    parent.inpman.with_key_states([&]() {
        if (parent.inpman.pressed(vkey::code::LEFT)) {
            posx -= dt * velocity;
        }
        if (parent.inpman.pressed(vkey::code::RIGHT)) {
            posx += dt * velocity;
        }
        if (parent.inpman.pressed(vkey::code::UP)) {
            posy -= dt * velocity;
        }
        if (parent.inpman.pressed(vkey::code::DOWN)) {
            posy += dt * velocity;
        }
    });
}

void demo::render(){
    glut::position pos{(int)posx - 32, (int)posx + 32, (int)posy - 48, (int)posy + 48};
    spr.render(parent, pos);
}
