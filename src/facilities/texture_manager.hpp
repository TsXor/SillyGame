#pragma once
#ifndef __TEXTURE_MANAGER__
#define __TEXTURE_MANAGER__

#include <unordered_map>
#include <optional>
#include <mutex>
#include "utilities/gpu_tex_img.hpp"
#include "res_loader.hpp"
#include "base_manager.hpp"
#include "utilities/swap_queue.hpp"
#include "utilities/naive_lru.hpp"

/**
 * 纹理管理器。管理纹理图片的加载过程。
 * 一些细节：
 *   - 纹理内存通过图片总大小统计
 *   - 这个类的代码很大程度上依赖于“纹理缓存至少足够保存当前使用的纹理”这个事实。
 *     如果纹理缓存过小，图片将不断重复加载，还可能发生一些不可预测的问题。
 */
class texture_manager : public base_manager {
    // GPU纹理缓存上限，单位：字节
    static constexpr size_t gpu_mem_thresh = 64 << 20; // MB
    // CPU纹理缓存上限，单位：字节
    static constexpr size_t cpu_mem_thresh = 256 << 20; // MB

    size_t cpu_mem = 0, gpu_mem = 0;
    naive_lru<stb_decoded_image> cpu_textures;
    naive_lru<gpu_tex2d> gpu_textures;
    std::mutex cpu_textures_lock;
    swap_queue<std::pair<std::string_view, stb_decoded_image*>> texture_load_queue;

    static void loop_job(game_window& wnd);
    uvco::coro_fn<void> texture_to_queue(const char* path);

public:
    texture_manager(game_window& parent);
    ~texture_manager();

    // 同步地加载指定路径的纹理，完全不使用缓存，适用于全局静态纹理
    std::optional<gpu_tex2d> wait_texture(const char* path);
    // 让IO线程加载指定路径的纹理到GPU缓存
    void want_texture(const char* path);
    // 获取指定路径的纹理，未加载完成时返回空指针
    gpu_tex2d* get_texture(const std::string_view& path);
};

#endif // __TEXTURE_MANAGER__
