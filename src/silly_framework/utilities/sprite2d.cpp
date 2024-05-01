#include "silly_framework/utilities/sprite2d.hpp"
#include "silly_framework/facilities/game_window.hpp"

using namespace silly_framework;

glm::mat4 sprite2d::tex_mat(unsigned int width, unsigned int height) const {
    return glut::uv_trans(uvpos, width, height, glut::eye4);
}

void sprite2d::render(game_window& wnd, const glut::position& xy, const glm::mat4& transform) const {
    auto&& [vs_w, vs_h] = wnd.renman.vs_size();
    if (xy.left >= vs_w || xy.right < 0 || xy.top >= vs_h || xy.bottom < 0) { return; }
    auto maybe_img = wnd.texman.get_texture(file_path);
    if (!maybe_img) { return; }
    auto& img = *maybe_img;
    // 由于文件名不变，可以认为每次传入的width和height相同
    if (!cache_tex_mat) { cache_tex_mat = tex_mat(img.width, img.height); }
    wnd.renman.blit(img.tex,
        glut::xy_trans(xy, vs_w, vs_h, transform),
        *cache_tex_mat
    );
}

void sprite2d::render(game_window &wnd, int cx, int cy, double scalev, const glm::mat4 &transform) const {
    auto xy = uvpos;
    xy.left   = double(xy.left)   * scalev;
    xy.right  = double(xy.right)  * scalev;
    xy.top    = double(xy.top)    * scalev;
    xy.bottom = double(xy.bottom) * scalev;
    xy -= xy.center();
    xy += glut::coord(cx, cy);
    auto&& [vs_w, vs_h] = wnd.renman.vs_size();
    xy += glut::coord(vs_w / 2, vs_h / 2);
    render(wnd, xy, transform);
}
