#include <cmath>
#include <numbers>
#include <filesystem>

#include <uvpp.hpp>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>

#include "ogl_main_window.hpp"
#include <oglwrap/shapes/rectangle_shape.h>


base_main_window* main_wnd = nullptr;

class show_square {
private:
    // Defines a full screen rectangle (see oglwrap/shapes/rectangle_shape.h)
    gl::RectangleShape rectangle_shape;

public:
    show_square(): rectangle_shape({gl::RectangleShape::kPosition, gl::RectangleShape::kTexCoord}) {
        auto& prog = shaders::programs::square();
        (prog | "pos").bindLocation(gl::RectangleShape::kPosition);
        (prog | "tex_coord").bindLocation(gl::RectangleShape::kTexCoord);
        gl::UniformSampler(prog, "tex") = 0;
    }
    ~show_square() = default;
    
    void render(){
        gl::Enable(gl::kBlend);
        gl::BlendFunc(gl::kSrcAlpha, gl::kOneMinusSrcAlpha);

        auto& prog = shaders::programs::square();
        gl::Use(prog);
        
        gl::ClearColor(0.2f, 0.2f, 0.2f, 1.0f);

        constexpr double period = 5;
        double theta = (fmod(glfwGetTime(), period) / period) * std::numbers::pi;
        gl::Uniform<glm::vec2> rotate_vec(prog, "rotate");
        rotate_vec.set(glm::vec2(cos(theta), sin(theta)));
        
        main_wnd->center_viewport(600, 600);
        main_wnd->use_texture("res/char.png");
        rectangle_shape.render();
        main_wnd->full_viewport();
        
        gl::UnuseProgram();
    }
};

class main_window : public base_main_window {
    show_square square;
public:
    main_window() : base_main_window("simple demo", 60, 10) { want_texture("res/char.png"); }
    void render() { square.render(); }
};

int main(int argc, char** argv) {
    // Windows特有的字符编码仪式
    uvpp::make_windows_encoding_happy(argc, argv);
    // 切换到程序文件所在文件夹
    std::filesystem::path program_path(argv[0]);
    std::filesystem::current_path(program_path.parent_path());
    // 设置默认日志记录器
    spdlog::set_default_logger(spdlog::basic_logger_mt("default_logger", "silly.log", true));
    // 启动主窗口
    main_window mw; main_wnd = &mw; mw.run();
    return 0;
}
