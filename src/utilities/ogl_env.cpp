#include "ogl_env.hpp"
#include <mutex>

static size_t ctx_refcnt;
static std::mutex ctx_refcnt_lock;

static void ogl_ctor() {
    if (!glfwInit()) { throw std::runtime_error("Cannot init GLFW!"); }
}

static void ogl_dtor() { glfwTerminate(); }

ogl_ctx_ref::ogl_ctx_ref() {
    const std::lock_guard guard(ctx_refcnt_lock);
    if (ctx_refcnt == 0) { ogl_ctor(); }
    ctx_refcnt += 1;
}

ogl_ctx_ref::~ogl_ctx_ref() {
    const std::lock_guard guard(ctx_refcnt_lock);
    ctx_refcnt -= 1;
    if (ctx_refcnt == 0) { ogl_dtor(); }
}

ogl_window::ogl_window(const char* title, int gl_major, int gl_minor) {
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, gl_major);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, gl_minor);
    glfwWindowHint(GLFW_RESIZABLE, true);

    const GLFWvidmode * mon_mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
    hwnd = glfwCreateWindow(mon_mode->width / 2, mon_mode->height / 2, title, nullptr, nullptr);
    if (!hwnd) { throw std::runtime_error("Cannot create GLFW window!"); }
    glfwSetWindowPos(hwnd, mon_mode->width / 4, mon_mode->height / 4);
}

ogl_window::~ogl_window() {
    glfwDestroyWindow(hwnd);
}
