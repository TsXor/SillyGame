#pragma once
#ifndef __GPU_TEX_IMG__
#define __GPU_TEX_IMG__

#include "utilities/ogl_deps.hpp"
#include "utilities/stb_image_wrap.hpp"

// 将已读取的图片加载到GPU纹理，需要在主线程执行
gl::Texture2D load_texture2d_from_stb(const stb_decoded_image_view &img);

struct gpu_tex2d {
    int width, height, n_channels;
    gl::Texture2D tex;

    gpu_tex2d(const stb_decoded_image_view &cpu_img);
    ~gpu_tex2d();

    gpu_tex2d(const gpu_tex2d &) = delete;
    gpu_tex2d(gpu_tex2d &&) = default;
    gpu_tex2d& operator=(const gpu_tex2d &) = delete;
    gpu_tex2d& operator=(gpu_tex2d &&) = default;

    operator gl::Texture2D &() { return tex; }
    gl::Texture2D *operator->() { return &tex; }

    size_t mem_size() { return width * height * n_channels; }
};

#endif // __GPU_TEX_IMG__
