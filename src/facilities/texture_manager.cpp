#include "texture_manager.hpp"
#include <spdlog/spdlog.h>


texture_manager::texture_manager(res_loader& ldr) : resldr(ldr) {}
texture_manager::~texture_manager() {}

std::optional<gpu_tex2d> texture_manager::wait_texture(const char *path) {
    auto img = uvco::run_join(resldr.load_image(path));
    if (img) { return gpu_tex2d(*img); } else { return std::nullopt; }
}

uvco::coro_fn<void> texture_manager::texture_to_queue(const char* path) {
    if (gpu_textures.get(path) != nullptr) { co_return; }
    auto img_ptr = [&](){
        const std::lock_guard guard(cpu_textures_lock);
        return cpu_textures.get(path);
    }();
    if (img_ptr == nullptr) {
        auto img = co_await resldr.load_image(path);
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

void texture_manager::want_texture(const std::string& path) {
    auto&& [path_str, not_present] = available_or_loading.emplace(path);
    if (not_present) { uvco::unleash(texture_to_queue(path_str->c_str())); }
}

gpu_tex2d* texture_manager::get_texture(const std::string& path) {
    want_texture(path);
    return try_get_texture(path);
}

gpu_tex2d* texture_manager::try_get_texture(const std::string_view& path) {
    return gpu_textures.get(path);
}

void texture_manager::clear_texture_queue() {
    texture_load_queue.reap([&](decltype(texture_load_queue)::list_type& pending) {
        for (auto&& [id, img] : pending) {
            auto tsize = img->mem_size();
            gpu_textures.emplace(id, *img);
            gpu_mem += tsize;
        }
        while (gpu_mem > gpu_mem_thresh) {
            gpu_mem -= gpu_textures.oldest()->mem_size();
            available_or_loading.erase(*gpu_textures.oldest_name());
            gpu_textures.pop_oldest();
        }
    });
}
