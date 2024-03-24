#include "globals.hpp"
#include "main_frame.hpp"


show_square::show_square(): rectangle_shape({gl::RectangleShape::kPosition, gl::RectangleShape::kTexCoord}) {
    auto& prog = shaders::programs::square();
    (prog | "pos").bindLocation(gl::RectangleShape::kPosition);
    (prog | "tex_coord").bindLocation(gl::RectangleShape::kTexCoord);
    gl::UniformSampler(prog, "tex") = 0;
}
show_square::~show_square() = default;

void show_square::render(){
    main_wnd->center_viewport(1024, 768);
    auto& prog = shaders::programs::square();
    gl::Use(prog);

    constexpr double period = 5;
    double theta = (fmod(glfwGetTime(), period) / period) * std::numbers::pi;
    gl::Uniform<glm::vec2> rotate_vec(prog, "rotate");
    rotate_vec.set(glm::vec2(cos(theta), sin(theta)));

    main_wnd->use_texture("res/char.png");
    rectangle_shape.render();
}
