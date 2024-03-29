#include "show_square.hpp"
#include "facilities/game_window.hpp"
#include "utilities/static_sprites.hpp"


show_square::show_square(game_window& window) : base_activity(window),
spr{parent, sprites::self_portrait()} {
    parent.renman.vs_size(1024, 768);
}

show_square::~show_square() = default;

static inline double period_ratio(double x, double period) { return fmod(x, period) / period; }
static inline double interpolate(double lo, double hi, double ratio) { return lo + (hi - lo) * ratio; }

void show_square::render(){
    constexpr double period = 5;
    double ratio = period_ratio(glfwGetTime(), period);
    double theta = interpolate(-std::numbers::pi, std::numbers::pi, ratio);

    parent.renman.vscreen_viewport();
    auto&& [vs_w, vs_h] = parent.renman.vs_size();
    sprite2d::position pos{(int)vs_w / 2 - 256, (int)vs_w / 2 + 256, (int)vs_h / 2 - 256, (int)vs_h / 2 + 256};
    auto rot = glm::translate(eye_mat4, glm::vec3((int)vs_w / 2, (int)vs_h / 2, 0.0f));
    rot = glm::rotate(rot, (float)theta, glm::vec3(0.0f, 0.0f, 1.0f));
    rot = glm::translate(rot, glm::vec3(-(int)vs_w / 2, -(int)vs_h / 2, 0.0f));
    spr.blit(pos, rot);
}
