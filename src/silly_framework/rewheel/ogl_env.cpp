#include <mutex>
#include <stdexcept>
#include "silly_framework/rewheel/ogl_env.hpp"

using namespace silly_framework::glenv;
namespace impl = silly_framework::glenv;

static size_t ctx_refcnt;
static std::mutex ctx_refcnt_lock;

static void ogl_ctor() {
    if (!glfwInit()) { throw std::runtime_error("Cannot init GLFW!"); }
}

static void ogl_dtor() { glfwTerminate(); }

ctx_ref::ctx_ref() {
    const std::lock_guard guard(ctx_refcnt_lock);
    if (ctx_refcnt == 0) { ogl_ctor(); }
    ctx_refcnt += 1;
}

ctx_ref::~ctx_ref() {
    const std::lock_guard guard(ctx_refcnt_lock);
    ctx_refcnt -= 1;
    if (ctx_refcnt == 0) { ogl_dtor(); }
}

window::window(const char* title, int gl_major, int gl_minor) {
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, gl_major);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, gl_minor);
    glfwWindowHint(GLFW_RESIZABLE, true);

    const GLFWvidmode * mon_mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
    hwnd = glfwCreateWindow(mon_mode->width / 2, mon_mode->height / 2, title, nullptr, nullptr);
    if (!hwnd) { throw std::runtime_error("Cannot create GLFW window!"); }
    glfwSetWindowPos(hwnd, mon_mode->width / 4, mon_mode->height / 4);
}

window::~window() {
    glfwDestroyWindow(hwnd);
}
