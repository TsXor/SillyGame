#include "render_manager.hpp"
#include "game_window.hpp"
#include "utilities/macros.hpp"
#include "utilities/static_shaders.hpp"


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
    blit_data.with_buf_do([&](glut::vertex_obj& vert) {
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

void render_manager::blit(gl::Texture2D& tex, const glm::mat4& xy, const glm::mat4& uv) {
    auto& prog = shaders::programs::sprite(); gl::Use(prog);
    gl::Uniform<glm::mat4> var_pos_trans(prog, "pos_trans");
    gl::Uniform<glm::mat4> var_tex_trans(prog, "tex_trans");
    var_pos_trans.set(xy);
    var_tex_trans.set(uv);
    gl::Bind(tex);
    blit_data.with_obj_do([&](glut::vertex_obj& vert) {
        gl::DrawArrays(gl::PrimType::kTriangles, 0, STATIC_ARRAY_SIZE(blit_vertices));
    });
}

void render_manager::blit(const sprite2d& spr, const position& xy, const glm::mat4& transform) {
    auto maybe_img = wnd().texman.get_texture(spr.path());
    if (!maybe_img) { return; }
    auto& img = *maybe_img;
    blit(img.tex,
        glut::xy_trans(xy, vs_w, vs_h, transform),
        spr.tex_mat(img.width, img.height)
    );
}

void render_manager::blit(const sprite2d& spr, const position& xy) {
    blit(spr, xy, glut::eye4);
}

void render_manager::blit(const map2d& map, const position& uv) {
    auto maybe_img = wnd().texman.get_texture(map.path());
    if (!maybe_img) { return; }
    auto& img = *maybe_img;
    blit(img.tex,
        glut::eye4,
        glut::uv_trans(uv, img.width, img.height, glut::eye4)
    );
}
