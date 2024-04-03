#include <cmath>
#include <numbers>
#include <filesystem>

#include <uvpp.hpp>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>

#include "framework.hpp"
#include "activities/demo.hpp"


const std::string project_name = "SillyGame";

int main(int argc, char** argv) {
    // Windows特有的字符编码仪式
    uvpp::make_windows_encoding_happy(argc, argv);
    // 切换到资源文件所在文件夹
    std::filesystem::path program_path(argv[0]);
    auto root_path = program_path.parent_path();
    std::filesystem::current_path(root_path / "res");
    // 设置默认日志记录器
    auto logfile_path = root_path / (project_name + ".log");
    auto logger = spdlog::basic_logger_mt(project_name, logfile_path.string(), true);
    spdlog::set_default_logger(logger);
    // 启动主窗口
    game_window(project_name.c_str(), 60, std::chrono::milliseconds(5))
        .run<demo>();
    // 再见...
    return 0;
}