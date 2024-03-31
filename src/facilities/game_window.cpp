#include "game_window.hpp"
#include <algorithm>
#include <thread>
#include <spdlog/spdlog.h>
#include "utilities/os_misc.hpp"


game_window::game_window(const char* title, int fps_limit_, int poll_interval_ms_):
gl_wnd(title), fps_limit(fps_limit_), poll_interval_ms(poll_interval_ms_),
render_timer(poll_interval_ms_), tick_timer(poll_interval_ms_),
texman(*this), actman(*this), renman(*this), inpman(*this) {
    gl_wnd.user_pointer(this);
    gl_wnd.key_callback(game_window::key_callback);
    disable_ime();
}

game_window::~game_window() {}

void game_window::render_loop() {
    gl_wnd.use_ctx();
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        spdlog::error("FATAL: Cannot load OpenGL with GLAD!");
        gl_wnd.should_close(true); return;
    }
    double last_render_time = gl_wnd.time();
    bool have_prepared_frame = false;
    while (!gl_wnd.should_close() && !actman.empty()) {
        for (auto& jobf : loop_jobs) { jobf(*this); }
        // 如果之前渲染的帧用掉了，那么渲染一帧新的
        if (!have_prepared_frame) {
            gl::Clear().Color().Depth();
            renman.vscreen_viewport();
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
        // 等待一小会，降低CPU占用
        render_timer.wait();
    }
}

void game_window::tick_loop() {
    double last_time = gl_wnd.time();
    tick_timer.wait();
    while (!gl_wnd.should_close() && !actman.empty()) {
        double this_time = gl_wnd.time();
        actman.current().tick(this_time, last_time);
        last_time = this_time;
        tick_timer.wait();
    }
}

void game_window::real_run() {
    std::jthread
        renderer(game_window::render_loop, this),
        ticker(game_window::tick_loop, this);
    while (!gl_wnd.should_close() && !actman.empty()) {
        // 主线程用于处理事件
        gl_wnd.wait_events();
    }
}

void game_window::key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    auto& self = *reinterpret_cast<game_window*>(glfwGetWindowUserPointer(window));
    self.inpman.on_key_event(key, action, mods);
}
