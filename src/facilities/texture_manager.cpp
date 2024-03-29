#include "texture_manager.hpp"
#include "game_window.hpp"
#include <spdlog/spdlog.h>


texture_manager::texture_manager(game_window& parent):
    base_manager(parent) { add_job(loop_job); }
texture_manager::~texture_manager() {}

std::optional<gpu_tex2d> texture_manager::wait_texture(const char *path) {
    auto img = uvco::run_join(wnd().resldr.load_image(path));
    if (img) { return gpu_tex2d(*img); } else { return std::nullopt; }
}

uvco::coro_fn<void> texture_manager::texture_to_queue(const char* path) {
    if (gpu_textures.get(path) != nullptr) { co_return; }
    auto img_ptr = [&](){
        const std::lock_guard guard(cpu_textures_lock);
        return cpu_textures.get(path);
    }();
    if (img_ptr == nullptr) {
        auto img = co_await wnd().resldr.load_image(path);
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

void texture_manager::want_texture(const char* path) {
    uvco::unleash(texture_to_queue(path));
}

gpu_tex2d* texture_manager::get_texture(const std::string_view& path) {
    return gpu_textures.get(path);
}

// 主窗口需要执行这个函数来驱动，因为纹理只能在主线程载入GPU
void texture_manager::loop_job(game_window& wnd) {
    auto& self = wnd.texman;
    self.texture_load_queue.reap([&](decltype(texture_load_queue)::list_type& pending) {
        for (auto&& [id, img] : pending) {
            auto tsize = img->mem_size();
            self.gpu_textures.emplace(id, *img);
            self.gpu_mem += tsize;
        }
        while (self.gpu_mem > gpu_mem_thresh) {
            self.gpu_mem -= self.gpu_textures.oldest()->mem_size();
            self.gpu_textures.pop_oldest();
        }
    });
}
