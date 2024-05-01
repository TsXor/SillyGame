#pragma once
#ifndef __SFLIB_GLFW_CONTEXT__
#define __SFLIB_GLFW_CONTEXT__

#include <tuple>
#include "silly_framework/config.h"
#include "silly_framework/ogl/glfw.h"

namespace silly_framework::glenv {

/**
 * OpenGL上下文引用。
 * 第一个引用构造时，初始化OpenGL环境。
 * 最后一个引用析构时，关闭OpenGL环境。
 */
struct SF_EXPORT ctx_ref {
    ctx_ref();
    ~ctx_ref();
};

/**
 * 对GLFW窗口的简单包装。
 * 注：默认情况下，窗口初始化在处于屏幕中央且可以随意缩放，要改变这个行为，
 *     可以在对象构造后调用GLFW API修改窗口属性。
 */
struct SF_EXPORT window {
    using handle_type = GLFWwindow*;
    const ctx_ref ctx;
    handle_type hwnd;

    window(const char* title, int gl_major, int gl_minor);
    window(const char* title) : window(title, 4, 1) {}
    ~window();

    operator handle_type() { return hwnd; }
    void use_ctx() { glfwMakeContextCurrent(hwnd); }
    bool should_close() { return glfwWindowShouldClose(hwnd); }
    void should_close(bool val) { return glfwSetWindowShouldClose(hwnd, val); }
    void swap_buffers() { glfwSwapBuffers(hwnd); }
    static double time() { return glfwGetTime(); }
    static void poll_events() { glfwPollEvents(); }
    static void wait_events() { glfwWaitEvents(); }
    std::tuple<int, int> size() {
        std::tuple<int, int> result;
        glfwGetWindowSize(hwnd, &std::get<0>(result), &std::get<1>(result));
        return result;
    }
    void key_callback(GLFWkeyfun cb) { glfwSetKeyCallback(hwnd, cb); }
    void user_pointer(void* data) { glfwSetWindowUserPointer(hwnd, data); }
    void* user_pointer() { return glfwGetWindowUserPointer(hwnd); }
};

} // namespace silly_framework::glenv

#endif // __SFLIB_GLFW_CONTEXT__
