#pragma once
#ifndef __TEXTURE_MANAGER__
#define __TEXTURE_MANAGER__

#include <unordered_map>
#include <optional>
#include <mutex>
#include "ogl_deps.hpp"
#include "res_loader.hpp"
#include "utilities/swap_queue.hpp"
#include "utilities/naive_lru.hpp"

class texture_manager {
    // GPU纹理缓存上限，单位：字节
    static constexpr size_t gpu_mem_thresh = 64 << 20; // MB
    // CPU纹理缓存上限，单位：字节
    static constexpr size_t cpu_mem_thresh = 256 << 20; // MB
    // 注：纹理内存通过图片总大小统计
    // 再注：这个类的代码很大程度上依赖于“纹理缓存至少足够保存当前使用的纹理”这个事实。

    size_t cpu_mem = 0, gpu_mem = 0;
    naive_lru<stb_decoded_image> cpu_textures;
    naive_lru<std::pair<gl::Texture2D, size_t>> gpu_textures;
    std::mutex cpu_textures_lock;
    swap_queue<std::pair<std::string_view, stb_decoded_image*>> texture_load_queue;

    uvco::coro_fn<void> texture_to_queue(res_loader_thread& loader, const char* path);

public:
    // 将已读取的图片加载到GPU纹理，需要在主线程执行
    static gl::Texture2D load_texture(const stb_decoded_image& img);
    // 同步地加载指定路径的纹理，完全不使用缓存，适用于全局静态纹理
    std::optional<gl::Texture2D> wait_texture(res_loader_thread& loader, const char* path);
    // 让IO线程加载指定路径的纹理到GPU缓存
    void want_texture(res_loader_thread& loader, const char* path);
    // 获取指定路径的纹理，未加载完成时返回空指针
    gl::Texture2D* get_texture(const char* path);
    // 主窗口需要执行这个函数来驱动，因为纹理只能在主线程载入GPU
    void main_window_job();
};

#endif // __TEXTURE_MANAGER__