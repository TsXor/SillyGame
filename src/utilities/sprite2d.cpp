#include "sprite2d.hpp"
#include "facilities/game_window.hpp"


sprite2d::sprite2d(std::string_view path, const position& uv) : file_path(path), uvpos(uv) {}
sprite2d::~sprite2d() {}

const glm::mat4& sprite2d::tex_mat(unsigned int width, unsigned int height) const {
    // 由于文件名不变，可以认为每次传入的width和height相同
    if (!_tex_mat_known) {
        _tex_mat = glut::uv_trans(uvpos, width, height, glut::eye4);
        _tex_mat_known = true;
    }
    return _tex_mat;
}

void sprite2d::render(game_window& wnd, const position& xy, const glm::mat4& transform) const {
    auto maybe_img = wnd.texman.get_texture(file_path);
    if (!maybe_img) { return; }
    auto& img = *maybe_img;
    auto [vs_w, vs_h] = wnd.renman.vs_size();
    wnd.renman.blit(img.tex,
        glut::xy_trans(xy, vs_w, vs_h, transform),
        tex_mat(img.width, img.height)
    );
}
