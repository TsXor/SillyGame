
// This file is generated, do not modify.

#include "static_shaders.hpp"
#include "static_shader_inits.hpp"

using namespace std::string_literals;


gl::Program& shaders::programs::square() {
    static gl::Program prog = utils::init_program(utils::make_program(gl::Shader(gl::kFragmentShader, files::use_texture_frag()), gl::Shader(gl::kVertexShader, files::transform_shape_vert())), ::shaders::details::square_init);
    return prog;
}


gl::ShaderSource& shaders::files::use_texture_frag() {
    static gl::ShaderSource source = utils::make_source("use_texture.frag"s, "#version 330 core\n\nin vec2 trans_tex_coord;\nuniform sampler2D tex;\nout vec4 frag_color;\n\nvoid main() {\n    frag_color = texture(tex, vec2(trans_tex_coord.x, 1 - trans_tex_coord.y));\n}\n"s);
    return source;
}


gl::ShaderSource& shaders::files::transform_shape_vert() {
    static gl::ShaderSource source = utils::make_source("transform_shape.vert"s, "#version 330 core\n\nin vec2 pos;\nin vec2 tex_coord;\nuniform vec2 rotate;\nout vec2 trans_tex_coord;\n\nvoid main() {\n    // rotate vertices\n    vec2 rotated_pos = vec2(pos.x * rotate.x - pos.y * rotate.y, pos.x * rotate.y + pos.y * rotate.x);\n    // shrink the full screen rectangle to only half size\n    gl_Position = vec4(0.5 * rotated_pos.xy, 0, 1);\n    // pass tex_coord\n    trans_tex_coord = tex_coord;\n}\n"s);
    return source;
}

