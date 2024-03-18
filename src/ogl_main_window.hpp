#pragma once
#ifndef __OGL_MAINWIN__
#define __OGL_MAINWIN__

#include "ogl_deps.hpp"
#include <tuple>


/**
 * 主窗口基类。创建窗口并取得OpenGL上下文。
 * 程序窗口类重载render方法实现渲染即可。
 * 本来这个类是用不着运行时多态的，但是为了可读性就先这么实现吧。
 * 
 * 注：默认情况下，窗口初始化在处于屏幕中央且可以随意缩放，要改变这个行为，
 *     可以在子类构造函数中调用GLFW API修改窗口属性。
 */
class base_main_window {
protected:
    GLFWwindow *hwnd;
    int _fps_limit;

public:
    base_main_window(const char* title, int fps_limit);
    ~base_main_window();

    std::tuple<int, int> size();
    void center_viewport(int hint_width, int hint_height);
    void full_viewport();
    
    void run();
    virtual void render() = 0;
};


#endif // __OGL_MAINWIN__
