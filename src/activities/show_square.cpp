#include "show_square.hpp"
#include "facilities/game_window.hpp"
#include "utilities/static_sprites.hpp"


show_square::show_square(game_window& window) : base_activity(window),
spr{parent, sprites::container_portrait()} {
    parent.renman.vs_size(1024, 768);
}

show_square::~show_square() = default;

static inline double period_ratio(double x, double period) { return fmod(x, period) / period; }
static inline double interpolate(double lo, double hi, double ratio) { return lo + (hi - lo) * ratio; }

void show_square::render(){
    constexpr double period = 5;
    double ratio = period_ratio(glfwGetTime(), period);
    double theta = interpolate(-std::numbers::pi, std::numbers::pi, ratio);

    auto&& [vs_w, vs_h] = parent.renman.vs_size();
    glut::position pos{(int)vs_w / 2 - 256, (int)vs_w / 2 + 256, (int)vs_h / 2 - 384, (int)vs_h / 2 + 384};
    auto rot = glut::centered_rotate2d(glut::eye4, theta, vs_w / 2, vs_h / 2);
    spr.blit(pos, rot);
}
