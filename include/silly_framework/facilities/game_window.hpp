#pragma once
#ifndef __SF_GAME_WINDOW__
#define __SF_GAME_WINDOW__

#include <list>
#include <functional>
#include <chrono>
#include <memory>
#include "silly_framework/rewheel/ogl_env.hpp"
#include "silly_framework/facilities/texture_manager.hpp"
#include "silly_framework/facilities/activity_manager.hpp"
#include "silly_framework/facilities/render_manager.hpp"
#include "silly_framework/facilities/input_manager.hpp"

namespace silly_framework {

class res_jloader;

/**
 * 游戏窗口类。
 * 这个类主要负责将一众部件打包在一起，具体的功能由各个部件分别实现。
 */
class game_window {
    using base_duration = std::chrono::microseconds;

    void render_loop(std::stop_token stoken);
    void tick_loop(std::stop_token stoken);
    void activity_loop(iface_activity& start);
    void real_run(iface_activity& start);
    static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

protected:
    glenv::window gl_wnd;
    unsigned int fps_limit;
    base_duration ticker_interval;
    // 隔离资源加载器的实现
    std::unique_ptr<res_jloader> resldr;

public:
    texture_manager texman;
    activity_manager actman;
    render_manager renman;
    input_manager inpman;

    game_window(const char* title, unsigned int fps_limit_, base_duration ticker_interval_);
    // 自动转换其他duration
    template <typename Rep, typename Period> requires (!std::is_same_v<std::chrono::duration<Rep, Period>, base_duration>)
    game_window(const char* title, unsigned int fps_limit_, std::chrono::duration<Rep, Period> ticker_interval_):
        game_window(title, fps_limit_, std::chrono::duration_cast<base_duration>(ticker_interval_)) {}
    ~game_window();

    // SillyGame，启动！
    // 构造一个activity并以它为入口开始运行
    template <typename SceneT, typename... ArgTs>
    void run(ArgTs&&... args) {
        auto act = new SceneT(*this, std::forward<ArgTs>(args)...);
        real_run(*act);
    }
};

} // namespace silly_framework

#endif // __SF_GAME_WINDOW__
