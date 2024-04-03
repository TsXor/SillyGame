#include "map2d.hpp"
#include "framework/facilities/game_window.hpp"


map2d::map2d(std::string_view path) : file_path(path) {}
map2d::~map2d() {}

void map2d::render(game_window& wnd, const position& uv) {
    auto maybe_img = wnd.texman.get_texture(file_path);
    if (!maybe_img) { return; }
    auto& img = *maybe_img;
    wnd.renman.blit(img.tex,
        glut::eye4,
        glut::uv_trans(uv, img.width, img.height, glut::eye4)
    );
}
