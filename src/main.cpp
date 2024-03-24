#include <cmath>
#include <numbers>
#include <filesystem>

#include <uvpp.hpp>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>

#include "facilities/ogl_main_window.hpp"
#include "main_frame.hpp"


const std::string project_name = "SillyGame";

base_main_window* main_wnd = nullptr;

class main_window : public base_main_window {
    show_square square;
public:
    main_window() : base_main_window(project_name.c_str(), 60, 10) {
        want_texture("res/char.png");
    }
    void render() {
        gl::Enable(gl::kBlend);
        gl::BlendFunc(gl::kSrcAlpha, gl::kOneMinusSrcAlpha);
        gl::ClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        square.render();
    }
};

int main(int argc, char** argv) {
    // Windows特有的字符编码仪式
    uvpp::make_windows_encoding_happy(argc, argv);
    // 切换到程序文件所在文件夹
    std::filesystem::path program_path(argv[0]);
    std::filesystem::current_path(program_path.parent_path());
    // 设置默认日志记录器
    auto logger = spdlog::basic_logger_mt(project_name, project_name + ".log", true);
    spdlog::set_default_logger(logger);
    // 启动主窗口
    main_window mw; main_wnd = &mw; mw.run();
    // 再见...
    return 0;
}
