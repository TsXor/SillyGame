#include "ogl_main_window.hpp"
#include <oglwrap/shapes/rectangle_shape.h>
#include <cmath>
#include <numbers>


class show_square {
private:
    // Defines a full screen rectangle (see oglwrap/shapes/rectangle_shape.h)
    gl::RectangleShape rectangle_shape;

public:
    show_square() {
        // Bind the attribute position to the location that the RectangleShape uses
        // (Both use attribute 0 by default for position, so this call isn't necessary)
        (shaders::programs::square() | "pos").bindLocation(gl::RectangleShape::kPosition);
    }
    ~show_square() = default;

    void render(base_main_window* wnd) {
        auto& prog = shaders::programs::square();
        gl::Use(prog);
        
        gl::ClearColor(0.2f, 0.2f, 0.2f, 1.0f);

        constexpr double period = 5;
        double theta = (fmod(glfwGetTime(), period) / period) * std::numbers::pi;
        gl::Uniform<glm::vec2> rotate_vec(prog, "rotate");
        rotate_vec.set(glm::vec2(cos(theta), sin(theta)));
        wnd->center_viewport(600, 600);
        rectangle_shape.render();
        wnd->full_viewport();
        
        gl::UnuseProgram();
    }
};

class main_window : public base_main_window {
    show_square square;
public:
    main_window() : base_main_window("simple demo", 60) {}
    void render() { square.render(this); }
};

int main() {
    main_window().run();
    return 0;
}
