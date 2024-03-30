#pragma once
#ifndef __OGL_MAINWIN__
#define __OGL_MAINWIN__

#include <list>
#include <functional>
#include "utilities/ogl_env.hpp"
#include "utilities/naive_timer.hpp"
#include "res_loader.hpp"
#include "texture_manager.hpp"
#include "activity_manager.hpp"
#include "render_manager.hpp"

/**
 * 游戏窗口类。
 * 这个类主要负责将一众部件打包在一起，具体的功能由各个部件分别实现。
 * 除此以外，这个类还提供了一个“循环回调”的接口。
 */
class game_window {
    friend class texture_manager;
    friend class activity_manager;
    friend class render_manager;
    
    void real_run();

protected:
    using job_type = void(*)(game_window&);

    ogl_window gl_wnd;
    int fps_limit, poll_interval_ms;
    std::list<job_type> loop_jobs;
    naive_timer timer;
    res_jloader resldr;

public:
    texture_manager texman;
    activity_manager actman;
    render_manager renman;

    game_window(const char* title, int fps_limit_, int poll_interval_ms_);
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
