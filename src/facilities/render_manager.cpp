#include "render_manager.hpp"
#include "game_window.hpp"

render_manager::render_manager(game_window& parent) : base_manager(parent) {}
render_manager::~render_manager() {}

void render_manager::use_texture(const char* path) {
    auto tex = wnd().texman.get_texture(path);
    if (tex) { gl::Bind(*tex); } else { glBindTexture(GL_TEXTURE_2D, 0); }
}

void render_manager::center_viewport(int hint_width, int hint_height) {
    auto [cur_width, cur_height] = wnd().gl_wnd.size();
    int vp_w = std::min(cur_height * hint_width / hint_height, cur_width);
    int vp_h = std::min(cur_width * hint_height / hint_width, cur_height);
    int x_offset = (cur_width - vp_w) / 2;
    int y_offset = (cur_height - vp_h) / 2;
    gl::Viewport(x_offset, y_offset, vp_w, vp_h);
}

void render_manager::full_viewport() {
    auto [cur_width, cur_height] = wnd().gl_wnd.size();
    gl::Viewport(0, 0, cur_width, cur_height);
}
