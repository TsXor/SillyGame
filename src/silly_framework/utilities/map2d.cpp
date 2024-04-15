#include "silly_framework/utilities/map2d.hpp"
#include "silly_framework/facilities/game_window.hpp"

using namespace silly_framework;

coutils::generator<glm::mat4> map2d::tex_mats(unsigned int width, unsigned int height) const {
    for (auto&& [block_uv, paste_coord] : pos_parts) {
        co_yield glut::uv_trans(block_uv, width, height, glut::eye4);
    }
}

void map2d::render(game_window& wnd, const glut::position& uv) const {
    auto maybe_img = wnd.texman.get_texture(file_path);
    if (!maybe_img) { return; }
    auto& img = *maybe_img;
    // 由于文件名不变，可以认为每次传入的width和height相同
    if (cache_tex_mats.empty()) {
        cache_tex_mats.reserve(pos_parts.size());
        for (auto&& tex_mat : tex_mats(img.width, img.height)) {
            cache_tex_mats.push_back(tex_mat);
        }
    }
    for (size_t i = 0; i < pos_parts.size(); ++i) {
        auto&& [block_uv, paste_coord] = pos_parts[i];
        auto&& tex_mat = cache_tex_mats[i];
        auto paste_uv = block_uv - glut::coord(block_uv.left, block_uv.top) + paste_coord;
        if (uv.intersect(paste_uv)) {
            auto rel_uv = paste_uv - glut::coord(uv.left, uv.top);
            wnd.renman.blit(img.tex,
                glut::xy_trans(rel_uv, uv.width(), uv.height(), glut::eye4),
                tex_mat
            );
        }
    }
}

void map2d::render(game_window& wnd, int cx, int cy, double scalev) const {
    auto&& [vs_w, vs_h] = wnd.renman.vs_size();
    unsigned int sw = double(vs_w) / scalev;
    unsigned int sh = double(vs_h) / scalev;
    glut::position uv;
    uv.left   = cx - sw / 2;
    uv.top    = cy - sh / 2;
    uv.right  = uv.left + sw;
    uv.bottom = uv.top  + sh;
    render(wnd, uv);
}
