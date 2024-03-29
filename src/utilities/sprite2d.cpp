#include "sprite2d.hpp"
#include "facilities/game_window.hpp"


sprite2d::sprite2d(std::string_view path, const position& uv) : file_path(path), uv(uv) {}
sprite2d::~sprite2d() {}

const glm::mat4& sprite2d::tex_mat(int width, int height) const {
    if (!_tex_mat_known) {
        _tex_mat = glm::mat4(1.0f);
        // 以下操作倒着看。
        _tex_mat = glm::scale(_tex_mat, glm::vec3(0.5f, 0.5f, 1.0f));
        _tex_mat = glm::translate(_tex_mat, glm::vec3(1.0f, 1.0f, 0.0f));
        _tex_mat *= glm::ortho<float>(0.0f, width, height, 0.0f);
        _tex_mat = glm::translate(_tex_mat, glm::vec3(uv.left, uv.top, 0.0f));
        _tex_mat = glm::scale(_tex_mat, glm::vec3(uv.width(), uv.height(), 1.0f));
    }
    return _tex_mat;
}
