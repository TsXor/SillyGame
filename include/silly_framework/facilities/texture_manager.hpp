#pragma once
#ifndef __SF_TEXTURE_MANAGER__
#define __SF_TEXTURE_MANAGER__

#include <unordered_set>
#include <optional>
#include <mutex>
#include <coutils.hpp>
#include "silly_framework/config.h"
#include "silly_framework/rewheel/gpu_tex_img.hpp"
#include "silly_framework/rewheel/swap_queue.hpp"
#include "silly_framework/rewheel/naive_lru.hpp"

namespace silly_framework {

class res_loader;

/**
 * 纹理管理器。管理纹理图片的加载过程。
 * 一些细节：
 *   - 纹理内存通过图片总大小统计
 *   - 这个类的代码很大程度上依赖于“纹理缓存至少足够保存当前使用的纹理”这个事实。
 *     如果纹理缓存过小，图片将不断重复加载，还可能发生一些不可预测的问题。
 */
class SF_EXPORT texture_manager {
    // GPU纹理缓存上限，单位：字节
    static constexpr size_t gpu_mem_thresh = 64 << 20; // MB
    // CPU纹理缓存上限，单位：字节
    static constexpr size_t cpu_mem_thresh = 256 << 20; // MB

    res_loader& resldr;
    size_t cpu_mem = 0, gpu_mem = 0;
    rewheel::naive_lru<rewheel::stb_decoded_image> cpu_textures;
    rewheel::naive_lru<rewheel::gpu_tex2d> gpu_textures;
    std::mutex cpu_textures_lock;
    using cpu_texture_ref = std::pair<std::string_view, rewheel::stb_decoded_image*>;
    rewheel::swap_queue<cpu_texture_ref> texture_load_queue;
    std::unordered_set<std::string> available_or_loading;

    coutils::async_fn<void> texture_to_queue(const char* path);

public:
    texture_manager(res_loader& ldr);
    ~texture_manager();

    // 同步地加载指定路径的纹理，完全不使用缓存，适用于全局静态纹理
    std::optional<rewheel::gpu_tex2d> wait_texture(const char* path);
    // 尝试从缓存获取指定路径的纹理，失败时返回空指针
    rewheel::gpu_tex2d* try_get_texture(const std::string_view& path);
    // 提示IO线程加载指定路径的纹理到GPU缓存
    // 建议在已知将要用到某纹理时调用此函数
    void want_texture(const std::string& path);
    // 获取指定路径的纹理，失败时返回空指针
    // 若未开始加载，启动加载
    rewheel::gpu_tex2d* get_texture(const std::string& path);
    // 将正在排队的纹理载入GPU
    // 注意：此函数只能在渲染线程调用
    void clear_texture_queue();
};

} // namespace silly_framework

#endif // __SF_TEXTURE_MANAGER__
