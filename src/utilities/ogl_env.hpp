#pragma once
#ifndef __GLFW_CONTEXT__
#define __GLFW_CONTEXT__

#include <tuple>
#include "ogl_deps.hpp"

/**
 * OpenGL上下文引用。
 * 第一个引用构造时，初始化OpenGL环境。
 * 最后一个引用析构时，关闭OpenGL环境。
 */
struct ogl_ctx_ref {
    ogl_ctx_ref();
    ~ogl_ctx_ref();
};

/**
 * 对GLFW窗口的简单包装。
 * 注：默认情况下，窗口初始化在处于屏幕中央且可以随意缩放，要改变这个行为，
 *     可以在对象构造后调用GLFW API修改窗口属性。
 */
struct ogl_window {
    using handle_type = GLFWwindow*;
    const ogl_ctx_ref ctx;
    handle_type hwnd;

    ogl_window(const char* title, int gl_major, int gl_minor);
    ogl_window(const char* title) : ogl_window(title, 4, 1) {}
    ~ogl_window();

    operator handle_type() { return hwnd; }
    void use_ctx() { glfwMakeContextCurrent(hwnd); }
    bool should_close() { return glfwWindowShouldClose(hwnd); }
    void swap_buffers() { glfwSwapBuffers(hwnd); }
    static double time() { return glfwGetTime(); }
    static void poll_events() { glfwPollEvents(); }
    std::tuple<int, int> size() {
        std::tuple<int, int> result;
        glfwGetWindowSize(hwnd, &std::get<0>(result), &std::get<1>(result));
        return result;
    }
};

#endif // __GLFW_CONTEXT__
