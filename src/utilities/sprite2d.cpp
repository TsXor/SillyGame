#include "sprite2d.hpp"
#include "facilities/game_window.hpp"


sprite2d::sprite2d(std::string_view path, const position& uv) : file_path(path), uvpos(uv) {}
sprite2d::~sprite2d() {}

// 由于文件名不变，可以认为每次传入的width和height相同
const glm::mat4& sprite2d::tex_mat(unsigned int width, unsigned int height) const {
    if (!_tex_mat_known) {
        _tex_mat = glut::uv_trans(uvpos, width, height, glut::eye4);
        _tex_mat_known = true;
    }
    return _tex_mat;
}
