#include <cmath>
#include <numbers>
#include <filesystem>

#include <uvpp.hpp>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>

#include "ogl_deps.hpp"
#include <oglwrap/shapes/rectangle_shape.h>

#include "ogl_main_window.hpp"
#include "res_loader.hpp"


res_loader_thread res_loader;

class show_square {
private:
    // Defines a full screen rectangle (see oglwrap/shapes/rectangle_shape.h)
    gl::RectangleShape rectangle_shape;
    gl::Texture2D texture;

public:
    show_square(): rectangle_shape({gl::RectangleShape::kPosition, gl::RectangleShape::kTexCoord}) {
        auto& prog = shaders::programs::square();
        (prog | "pos").bindLocation(gl::RectangleShape::kPosition);
        (prog | "tex_coord").bindLocation(gl::RectangleShape::kTexCoord);
        gl::UniformSampler(prog, "tex") = 0;

        stb_decoded_image img;
        uvco::run_join(load_image(res_loader.uvloop, "res/char.png", img));
        gl::Bind(texture);
        texture.upload(gl::kSrgb8Alpha8, img.width, img.height, gl::kRgba, gl::kUnsignedByte, img.data);
        texture.minFilter(gl::kNearest); texture.magFilter(gl::kNearest);
        gl::Unbind(texture);
    }
    ~show_square() = default;

    void render(base_main_window* wnd) {
        gl::Enable(gl::kBlend);
        gl::BlendFunc(gl::kSrcAlpha, gl::kOneMinusSrcAlpha);

        auto& prog = shaders::programs::square();
        gl::Use(prog);
        
        gl::ClearColor(0.2f, 0.2f, 0.2f, 1.0f);

        constexpr double period = 5;
        double theta = (fmod(glfwGetTime(), period) / period) * std::numbers::pi;
        gl::Uniform<glm::vec2> rotate_vec(prog, "rotate");
        rotate_vec.set(glm::vec2(cos(theta), sin(theta)));
        
        wnd->center_viewport(600, 600);
        gl::Bind(texture);
        rectangle_shape.render();
        gl::Unbind(texture);
        wnd->full_viewport();
        
        gl::UnuseProgram();
    }
};

class main_window : public base_main_window {
    show_square square;
public:
    main_window() : base_main_window("simple demo", 60, 10) {}
    void render() { square.render(this); }
};

int main(int argc, char** argv) {
    uvpp::make_windows_encoding_happy(argc, argv);
    std::filesystem::path program_path(argv[0]);
    std::filesystem::current_path(program_path.parent_path());
    spdlog::set_default_logger(spdlog::basic_logger_mt("default_logger", "silly.log", true));
    res_loader.start();
    
    main_window().run();
    
    res_loader.stop();
    res_loader.join();
    return 0;
}
