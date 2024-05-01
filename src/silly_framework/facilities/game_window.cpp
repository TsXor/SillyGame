#include <algorithm>
#include <thread>
#include "silly_framework/settings/logger_all.hpp"
#include "silly_framework/facilities/game_window.hpp"
#include "silly_framework/facilities/res_loader.hpp"
#include "silly_framework/rewheel/naive_timer.hpp"
#include "silly_framework/rewheel/os_misc.hpp"

using namespace silly_framework;

game_window::game_window(const char* title, unsigned int fps_limit_, base_duration ticker_interval_):
gl_wnd(title), fps_limit(fps_limit_), ticker_interval(ticker_interval_), resldr(new res_jloader()),
texman(*resldr), actman(), renman(gl_wnd), inpman() {
    gl_wnd.user_pointer(this);
    gl_wnd.key_callback(game_window::key_callback);
    os_misc::disable_ime();
}

game_window::~game_window() {}

void game_window::render_loop(std::stop_token stoken) {
    gl_wnd.use_ctx();
    if (!sillyFrameworkGladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        logger::is()->error("FATAL: Cannot load OpenGL with GLAD!");
        gl_wnd.should_close(true); return;
    }
    double last_render_time = gl_wnd.time();
    bool have_prepared_frame = false;
    rewheel::naive_timer timer(ticker_interval);
    while (!stoken.stop_requested()) {
        rewheel::time_guard tg(timer);
        texman.clear_texture_queue();
        // 如果之前渲染的帧用掉了，那么渲染一帧新的
        if (!have_prepared_frame) {
            gl::Clear().Color().Depth();
            renman.vscreen_viewport();
            actman.render();
            have_prepared_frame = true;
        }
        // 如果距离上次显示渲染内容已经过了1/fps，那么
        // 交换缓冲区显示渲染内容并标记渲染的帧已使用
        if (gl_wnd.time() - last_render_time >= (1.0 / (double)fps_limit)) {
            gl_wnd.swap_buffers();
            last_render_time = gl_wnd.time();
            have_prepared_frame = false;
        }
    }
}

void game_window::tick_loop(std::stop_token stoken) {
    double last_time = gl_wnd.time();
    rewheel::naive_timer timer(ticker_interval);
    while (!stoken.stop_requested()) {
        rewheel::time_guard tg(timer);
        double this_time = gl_wnd.time();
        actman.post_until(tg.limit,
            [=](std::vector<iface_activity*>& stack) {
                stack.back()->on_tick(this_time, last_time);
            }
        );
        last_time = this_time;
    }
}

void game_window::activity_loop(silly_framework::iface_activity &start) {
    actman.loop(start);
    if (!gl_wnd.should_close()) {
        gl_wnd.should_close(true);
    }
}

void game_window::real_run(iface_activity& start) {
    std::jthread
        renderer(render_loop, this),
        ticker(tick_loop, this),
        actor(activity_loop, this, std::ref(start));
    while (!gl_wnd.should_close()) {
        // 主线程用于处理事件
        gl_wnd.wait_events();
    }
    actman.post_stop();
}

constexpr auto input_deadline = std::chrono::milliseconds(5);

void game_window::key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    auto& self = *reinterpret_cast<game_window*>(glfwGetWindowUserPointer(window));
    bool did_update = self.inpman.update_key_state(key, scancode, action, mods);
    bool is_vkey = self.inpman.has_vkey(key);
    self.actman.post_timeout(input_deadline,
        [=, &self](std::vector<iface_activity*>& stack) {
            if (!(stack.back()->vkey_only() && !is_vkey)) {
                self.inpman.with_key_states([&]() {
                    stack.back()->on_key_signal();
                    if (did_update) { stack.back()->on_key_change(); }
                });
            }
        }
    );
}
