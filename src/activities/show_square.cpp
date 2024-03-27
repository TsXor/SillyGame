#include "show_square.hpp"
#include "facilities/game_window.hpp"
#include "utilities/static_shaders.hpp"


show_square::show_square(game_window& window_) : window(window_),
rectangle_shape({gl::RectangleShape::kPosition, gl::RectangleShape::kTexCoord}) {
    window.texman.want_texture("res/char.png");
}
show_square::~show_square() = default;

static inline double period_ratio(double x, double period) { return fmod(x, period) / period; }
static inline double interpolate(double lo, double hi, double ratio) { return lo + (hi - lo) * ratio; }

void show_square::render(){
    window.renman.center_viewport(512, 512);
    auto& prog = shaders::programs::square();
    gl::Use(prog);

    constexpr double period = 5;
    double ratio = period_ratio(glfwGetTime(), period);
    double theta = interpolate(-std::numbers::pi, std::numbers::pi, ratio);
    gl::Uniform<glm::vec2> rotate_vec(prog, "rotate");
    rotate_vec.set(glm::vec2(cos(theta), sin(theta)));

    window.renman.use_texture("res/char.png");
    rectangle_shape.render();
}
