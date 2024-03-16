#pragma once
#ifndef __OGL_MAINWIN__
#define __OGL_MAINWIN__

#include "ogl_deps.hpp"


/**
 * 主窗口基类。创建窗口并取得OpenGL上下文。
 * 程序窗口类重载render方法实现渲染即可。
 * 本来这个类是用不着运行时多态的，但是为了可读性就先这么实现吧。
 */
class base_main_window {
protected:
    GLFWwindow *hwnd;
    int _width; int _height;

public:
    base_main_window(int width, int height, const char* title);
    ~base_main_window();

    int width() { return _width; }
    int height() { return _height; }
    
    void run();
    virtual void render() = 0;
};


#endif // __OGL_MAINWIN__
