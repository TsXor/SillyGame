#include "game_window.hpp"
#include <algorithm>
#include <thread>


game_window::game_window(const char* title, int fps_limit_, int poll_interval_ms_):
gl_wnd(title), fps_limit(fps_limit_), poll_interval_ms(poll_interval_ms_),
texman(*this), actman(*this), renman(*this) { gl_wnd.use_ctx(); }

game_window::~game_window() {}

void game_window::real_run() {
    // 限制fps，给显卡减点工作量
    double last_render_time = gl_wnd.time();
    bool have_prepared_frame = false;
    renman.vscreen_viewport();
    while (!gl_wnd.should_close() && !actman.empty()) {
        for (auto& jobf : loop_jobs) { jobf(*this); }
        // 如果之前渲染的帧用掉了，那么渲染一帧新的
        if (!have_prepared_frame) {
            gl::Clear().Color().Depth();
            actman.current().render();
            have_prepared_frame = true;
        }
        // 如果距离上次显示渲染内容已经过了1/fps，那么
        // 交换缓冲区显示渲染内容并标记渲染的帧已使用
        if (gl_wnd.time() - last_render_time >= (1.0 / (double)fps_limit)) {
            gl_wnd.swap_buffers();
            last_render_time = gl_wnd.time();
            have_prepared_frame = false;
        }
        // 处理事件
        gl_wnd.poll_events();
        // 等待一小会，降低CPU占用
        std::this_thread::sleep_for(std::chrono::milliseconds(poll_interval_ms));
    }
}
