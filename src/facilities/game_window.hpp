#pragma once
#ifndef __OGL_MAINWIN__
#define __OGL_MAINWIN__

#include <list>
#include <functional>
#include <chrono>
#include "utilities/ogl_env.hpp"
#include "res_loader.hpp"
#include "texture_manager.hpp"
#include "activity_manager.hpp"
#include "render_manager.hpp"
#include "input_manager.hpp"

/**
 * 游戏窗口类。
 * 这个类主要负责将一众部件打包在一起，具体的功能由各个部件分别实现。
 * 除此以外，这个类还提供了一个“循环回调”的接口。
 * 笑点解析：不算libuv再创建的，这个类一共启动了5个线程。
 */
class game_window {
    using base_duration = std::chrono::microseconds;

    friend class texture_manager;
    friend class activity_manager;
    friend class render_manager;
    friend class input_manager;

    void render_loop();
    void tick_loop();
    void real_run();
    static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

protected:
    using job_type = void(*)(game_window&);

    ogl_window gl_wnd;
    unsigned int fps_limit;
    std::chrono::microseconds ticker_interval;
    std::list<job_type> loop_jobs;
    res_jloader resldr;

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

    using job_handle_type = decltype(loop_jobs)::iterator;

    job_handle_type add_loop_job(job_type fn) {
        return loop_jobs.emplace(loop_jobs.end(), fn);
    }
    void remove_loop_job(job_handle_type job) {
        loop_jobs.erase(job);
    }

    // SillyGame，启动！
    // 构造一个activity并以它为入口开始运行
    template <typename SceneT, typename... ArgTs>
    void run(ArgTs... args) {
        actman.emplace<SceneT>(std::forward<ArgTs>(args)...);
        real_run();
    }
};

#endif // __OGL_MAINWIN__
