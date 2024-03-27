#include <cmath>
#include <numbers>
#include <filesystem>

#include <uvpp.hpp>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>

#include "facilities/game_window.hpp"
#include "activities/show_square.hpp"


const std::string project_name = "SillyGame";

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
    game_window(project_name.c_str(), 60, 10).run<show_square>();
    // 再见...
    return 0;
}
