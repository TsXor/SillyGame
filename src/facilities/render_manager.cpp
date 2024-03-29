#include "render_manager.hpp"
#include "game_window.hpp"
#include "utilities/macros.hpp"
#include "utilities/static_shaders.hpp"


extern const glm::mat4 eye_mat4 = glm::mat4(1.0f);

static const float blit_vertices[] = {
    // pos      // tex
    0.0f, 1.0f, 0.0f, 1.0f,
    1.0f, 0.0f, 1.0f, 0.0f,
    0.0f, 0.0f, 0.0f, 0.0f, 
    0.0f, 1.0f, 0.0f, 1.0f,
    1.0f, 1.0f, 1.0f, 1.0f,
    1.0f, 0.0f, 1.0f, 0.0f
};

render_manager::render_manager(game_window& parent) : base_manager(parent) {
    blit_data.with_buf_do([&](gl_vertex& vert) {
        gl::VertexAttrib(0).pointer(4, gl::kFloat, false, 0, 0).enable();
        vert.vbo.data(sizeof(blit_vertices), blit_vertices);
    });
}
render_manager::~render_manager() {}

void render_manager::use_texture(const std::string_view& path) {
    auto img = wnd().texman.get_texture(path);
    if (img) { gl::Bind(img->tex); } else { glBindTexture(GL_TEXTURE_2D, 0); }
}

void render_manager::center_viewport(unsigned int hint_width, unsigned int hint_height) {
    auto [cur_width, cur_height] = wnd().gl_wnd.size();
    int vp_w = std::min(cur_height * (int)hint_width / (int)hint_height, cur_width);
    int vp_h = std::min(cur_width * (int)hint_height / (int)hint_width, cur_height);
    int x_offset = (cur_width - vp_w) / 2;
    int y_offset = (cur_height - vp_h) / 2;
    gl::Viewport(x_offset, y_offset, vp_w, vp_h);
}

void render_manager::vscreen_viewport() {
    if (vs_w == 0 || vs_h == 0) { throw std::logic_error("vscreen size not set"); }
    center_viewport(vs_w, vs_h);
}

void render_manager::full_viewport() {
    auto [cur_width, cur_height] = wnd().gl_wnd.size();
    gl::Viewport(0, 0, cur_width, cur_height);
}

void render_manager::blit(const sprite2d& spr, const position& xy, const glm::mat4& transform) {
    auto maybe_img = wnd().texman.get_texture(spr.file_path);
    if (!maybe_img) { return; }
    auto& img = *maybe_img;

    // 以下操作倒着看。
    glm::mat4 pos = glm::ortho<float>(0.0f, vs_w, vs_h, 0.0f) * transform;
    pos = glm::translate(pos, glm::vec3(xy.left, xy.top, 0.0f)); 
    pos = glm::scale(pos, glm::vec3(xy.width(), xy.height(), 1.0f)); 

    auto& prog = shaders::programs::sprite(); gl::Use(prog);
    gl::Uniform<glm::mat4> var_pos_trans(prog, "pos_trans");
    gl::Uniform<glm::mat4> var_tex_trans(prog, "tex_trans");
    var_pos_trans.set(pos);
    var_tex_trans.set(spr.tex_mat(img.width, img.height));
    gl::Bind(img.tex);
    blit_data.with_obj_do([&](gl_vertex& vert) {
        gl::DrawArrays(gl::PrimType::kTriangles, 0, STATIC_ARRAY_SIZE(blit_vertices));
    });
}

void render_manager::blit(const sprite2d& spr, const position& xy) {
    blit(spr, xy, eye_mat4);
}
