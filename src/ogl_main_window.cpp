#include "ogl_main_window.hpp"

base_main_window::base_main_window(int width, int height, const char* title):
_width(width), _height(height) {
    if (!glfwInit()) { throw std::runtime_error("Cannot init GLFW!"); }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_RESIZABLE, false);

    hwnd = glfwCreateWindow(width, height, title, nullptr, nullptr);
    if (!hwnd) { throw std::runtime_error("Cannot create GLFW window!"); }

    glfwMakeContextCurrent(hwnd);
    if (!gladLoadGL()) { throw std::runtime_error("Cannot do load OpenGL with GLAD!"); }
}

base_main_window::~base_main_window() { glfwTerminate(); }
    
void base_main_window::run() {
    while (!glfwWindowShouldClose(hwnd)) {
        gl::Clear().Color().Depth();
        render();
        glfwSwapBuffers(hwnd);
        glfwPollEvents();
    }
}
