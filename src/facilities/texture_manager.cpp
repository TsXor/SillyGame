#include "texture_manager.hpp"
#include <spdlog/spdlog.h>


gl::Texture2D texture_manager::load_texture(const stb_decoded_image& img) {
    gl::Texture2D texture;
    
    gl::enums::PixelDataInternalFormat internal_format;
    gl::enums::PixelDataFormat format;
    switch (img.n_channels) {
        case 1:
            internal_format = gl::kR8;
            format = gl::kRed;
            break;
        case 2:
            internal_format = gl::kRg8;
            format = gl::kRg;
            break;
        case 3:
            internal_format = gl::kSrgb8;
            format = gl::kRgb;
            break;
        case 4:
            internal_format = gl::kSrgb8Alpha8;
            format = gl::kRgba;
            break;
        // 不应该发生，因为加载图片时已验证
        default: return texture;
    }
    
    gl::Bind(texture);
    texture.upload(internal_format, img.width, img.height, format, gl::kUnsignedByte, img.data);
    // 默认使用最近邻缩放
    texture.minFilter(gl::kNearest); texture.magFilter(gl::kNearest);
    gl::Unbind(texture);
    return texture;
}

std::optional<gl::Texture2D> texture_manager::wait_texture(res_loader_thread& loader, const char *path) {
    auto img = uvco::run_join(loader.load_image(path));
    if (img) { return load_texture(*img); } else { return std::nullopt; }
}

uvco::coro_fn<void> texture_manager::texture_to_queue(res_loader_thread& loader, const char* path) {
    if (gpu_textures.get(path) != nullptr) { co_return; }
    auto img_ptr = [&](){
        const std::lock_guard guard(cpu_textures_lock);
        return cpu_textures.get(path);
    }();
    if (img_ptr == nullptr) {
        auto img = co_await loader.load_image(path);
        if (img) {
            const std::lock_guard guard(cpu_textures_lock);
            img_ptr = &cpu_textures.emplace(path, *std::move(img));
            cpu_mem += img_ptr->mem_size();
            while (cpu_mem > cpu_mem_thresh) {
                cpu_mem -= cpu_textures.oldest()->mem_size();
                cpu_textures.pop_oldest();
            }
        } else { co_return; }
    }
    texture_load_queue.emplace(path, img_ptr);
}

void texture_manager::want_texture(res_loader_thread& loader, const char* path) {
    uvco::unleash(texture_to_queue(loader, path));
}

gl::Texture2D* texture_manager::get_texture(const char* path) {
    return &(gpu_textures.get(path)->first);
}

void texture_manager::main_window_job() {
    texture_load_queue.reap([&](decltype(texture_load_queue)::list_type& pending) {
        for (auto&& info : pending) {
            auto tsize = info.second->mem_size();
            this->gpu_textures.emplace(info.first, load_texture(*info.second), tsize);
            this->gpu_mem += tsize;
        }
        while (this->gpu_mem > gpu_mem_thresh) {
            this->gpu_mem -= this->gpu_textures.oldest()->second;
            this->gpu_textures.pop_oldest();
        }
    });
}
