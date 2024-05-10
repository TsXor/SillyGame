#include <cmath>
#include <numbers>
#include <filesystem>

#include "silly_framework.hpp"
#include "silly_framework/rewheel/os_misc.hpp"
#include "silly_framework/settings/logger.hpp"
#include "activities/avg_scene.hpp"

namespace sf = silly_framework;

const std::string project_name = "SillyGame";

int main(int argc, char** argv) {
    // Windows特有的字符编码仪式
    sf::os_misc::encoding_spell(argc, argv);
    // 切换到资源文件所在文件夹
    std::filesystem::path program_path(argv[0]);
    auto root_path = program_path.parent_path();
    std::filesystem::current_path(root_path / "res");
    // 设置默认日志记录器
    auto logfile_path = root_path / (project_name + ".log");
    sf::logger::make(project_name, logfile_path.string());
    // 启动主窗口
    sf::game_window(project_name.c_str(), 60, std::chrono::milliseconds(5))
        .run<acts::avg_scene>("dorm_room", "");
    // 再见...
    return 0;
}
