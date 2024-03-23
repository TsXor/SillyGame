#include "ogl_main_window.hpp"
#include <algorithm>
#include <thread>


base_main_window::base_main_window(const char* title, int fps_limit_, int poll_interval_ms_):
fps_limit(fps_limit_), poll_interval_ms(poll_interval_ms_) {
    if (!glfwInit()) { throw std::runtime_error("Cannot init GLFW!"); }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_RESIZABLE, true);

    const GLFWvidmode * mon_mode = glfwGetVideoMode(glfwGetPrimaryMonitor());

    hwnd = glfwCreateWindow(mon_mode->width / 2, mon_mode->height / 2, title, nullptr, nullptr);
    glfwSetWindowPos(hwnd, mon_mode->width / 4, mon_mode->height / 4);
    if (!hwnd) { throw std::runtime_error("Cannot create GLFW window!"); }

    glfwMakeContextCurrent(hwnd);
    if (!gladLoadGL()) { throw std::runtime_error("Cannot do load OpenGL with GLAD!"); }
}

base_main_window::~base_main_window() { glfwTerminate(); }

std::tuple<int, int> base_main_window::size() {
    std::tuple<int, int> result;
    glfwGetWindowSize(hwnd, &std::get<0>(result), &std::get<1>(result));
    return result;
}

void base_main_window::center_viewport(int hint_width, int hint_height) {
    auto [cur_width, cur_height] = size();
    int vp_w = std::min(cur_height * hint_width / hint_height, cur_width);
    int vp_h = std::min(cur_width * hint_height / hint_width, cur_height);
    int x_offset = (cur_width - vp_w) / 2;
    int y_offset = (cur_height - vp_h) / 2;
    gl::Viewport(x_offset, y_offset, vp_w, vp_h);
}

void base_main_window::full_viewport() {
    auto [cur_width, cur_height] = size();
    gl::Viewport(0, 0, cur_width, cur_height);
}

void base_main_window::run() {
    // 限制fps，给显卡减点工作量
    double last_render_time = glfwGetTime();
    bool have_prepared_frame = false;
    while (!glfwWindowShouldClose(hwnd)) {
        // 如果之前渲染的帧用掉了，那么渲染一帧新的
        if (!have_prepared_frame) {
            gl::Clear().Color().Depth();
            render();
            have_prepared_frame = true;
        }
        // 如果距离上次显示渲染内容已经过了1/fps，那么
        // 交换缓冲区显示渲染内容并标记渲染的帧已使用
        if (glfwGetTime() - last_render_time >= (1.0 / (double)fps_limit)) {
            glfwSwapBuffers(hwnd);
            last_render_time = glfwGetTime();
            have_prepared_frame = false;
        }
        glfwPollEvents();
        std::this_thread::sleep_for(std::chrono::milliseconds(poll_interval_ms));
    }
}
