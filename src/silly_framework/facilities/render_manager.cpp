#include <array>
#include "silly_framework/static_shaders.hpp"
#include "silly_framework/facilities/render_manager.hpp"

using namespace silly_framework;

static const std::array blit_vertices{
    // pos      // tex
    0.0f, 1.0f, 0.0f, 1.0f,
    1.0f, 0.0f, 1.0f, 0.0f,
    0.0f, 0.0f, 0.0f, 0.0f, 
    0.0f, 1.0f, 0.0f, 1.0f,
    1.0f, 1.0f, 1.0f, 1.0f,
    1.0f, 0.0f, 1.0f, 0.0f
};

render_manager::render_manager(glenv::window& gl_wnd_) : gl_wnd(gl_wnd_) {}
render_manager::~render_manager() {}

glut::vertex_obj& render_manager::blit_data() {
    if (!lazy_blit_data) {
        lazy_blit_data.emplace();
        lazy_blit_data->with_buf_do([&](glut::vertex_obj& vert) {
            gl::VertexAttrib(0).pointer(4, gl::kFloat, false, 0, 0).enable();
            vert.vbo.data(sizeof(blit_vertices), blit_vertices.data());
        });
    }
    return *lazy_blit_data;
}

void render_manager::center_viewport(unsigned int hint_width, unsigned int hint_height) {
    auto [cur_width, cur_height] = gl_wnd.size();
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
    auto [cur_width, cur_height] = gl_wnd.size();
    gl::Viewport(0, 0, cur_width, cur_height);
}

void render_manager::blit(gl::Texture2D& tex, const glm::mat4& xy, const glm::mat4& uv) {
    gl::Enable(gl::kBlend);
    gl::BlendFunc(gl::kSrcAlpha, gl::kOneMinusSrcAlpha);
    auto& prog = shaders::programs::sprite(); gl::Use(prog);
    gl::Uniform<glm::mat4> var_pos_trans(prog, "pos_trans");
    gl::Uniform<glm::mat4> var_tex_trans(prog, "tex_trans");
    var_pos_trans.set(xy);
    var_tex_trans.set(uv);
    gl::Bind(tex);
    blit_data().with_obj_do([&](glut::vertex_obj& vert) {
        gl::DrawArrays(gl::PrimType::kTriangles, 0, blit_vertices.size());
    });
}
