#include "ogl_main_window.hpp"
#include <oglwrap/shapes/rectangle_shape.h>

class show_square {
private:
    // Defines a full screen rectangle (see oglwrap/shapes/rectangle_shape.h)
    gl::RectangleShape rectangle_shape;
    gl::Program prog; // A shader program

public:
    show_square() {
        // Create a vertex shader
        gl::ShaderSource vs_source;
        vs_source.set_source(R"""(
            #version 330 core
            in vec2 pos;
            void main() {
                // Shrink the full screen rectangle to only half size
                gl_Position = vec4(0.5 * pos.xy, 0, 1);
            }
        )""");
        // Give a name for the shader (will be displayed in diagnostic messages)
        vs_source.set_source_file("example_shader.vert");
        gl::Shader vs(gl::kVertexShader, vs_source);

        // Create a fragment shader
        gl::ShaderSource fs_source;
        fs_source.set_source(R"""(
            #version 330 core
            out vec4 fragColor;
            void main() {
                fragColor = vec4(0.0, 1.0, 1.0, 1.0);
            }
        )""");
        // Give a name for the shader (will be displayed in diagnostic messages)
        fs_source.set_source_file("example_shader.frag");
        gl::Shader fs(gl::kFragmentShader, fs_source);

        // Create the shader program
        prog.attachShader(vs);
        prog.attachShader(fs);
        prog.link();
    }
    ~show_square() = default;

    void render() {
        gl::Use(prog);
        // Bind the attribute position to the location that the RectangleShape uses
        // (Both use attribute 0 by default for position, so this call isn't necessary)
        (prog | "pos").bindLocation(gl::RectangleShape::kPosition);
        // Set the clear color to grey
        gl::ClearColor(0.2f, 0.2f, 0.2f, 1.0f);
        rectangle_shape.render();
        gl::Unuse(prog);
    }
};

class main_window : public base_main_window {
    show_square square;
public:
    main_window() : base_main_window(600, 600) {}
    void render() { square.render(); }
};

int main() {
    main_window().run();
    return 0;
}
