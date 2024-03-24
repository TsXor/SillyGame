
// This file is generated, do not modify.

#ifndef __GENERATED_SHADERS_DEF__
#define __GENERATED_SHADERS_DEF__

namespace shaders {

using namespace std::string_literals;

namespace utils {

static inline gl::ShaderSource builtin_source(const std::string& name, const std::string& text) {
    gl::ShaderSource source;
    source.set_source(text);
    source.set_source_file(name);
    return source;
}

template <typename... Shaders>
static inline gl::Program builtin_program(Shaders&&... shaders) {
    gl::Program prog;
    (..., prog.attachShader(shaders));
    prog.link();
    return prog;
}

} // namespace utils

namespace files {

static inline gl::ShaderSource& transform_shape_vert() {
    static gl::ShaderSource source = utils::builtin_source("transform_shape.vert"s, "#version 330 core\n\nin vec2 pos;\nin vec2 tex_coord;\nuniform vec2 rotate;\nout vec2 trans_tex_coord;\n\nvoid main() {\n    // rotate vertices\n    vec2 rotated_pos = vec2(pos.x * rotate.x - pos.y * rotate.y, pos.x * rotate.y + pos.y * rotate.x);\n    // shrink the full screen rectangle to only half size\n    gl_Position = vec4(0.5 * rotated_pos.xy, 0, 1);\n    // pass tex_coord\n    trans_tex_coord = tex_coord;\n}\n"s);
    return source;
}


static inline gl::ShaderSource& use_texture_frag() {
    static gl::ShaderSource source = utils::builtin_source("use_texture.frag"s, "#version 330 core\n\nin vec2 trans_tex_coord;\nuniform sampler2D tex;\nout vec4 frag_color;\n\nvoid main() {\n    frag_color = texture(tex, vec2(trans_tex_coord.x, 1 - trans_tex_coord.y));\n}\n"s);
    return source;
}

} // namespace files

namespace programs {

static inline gl::Program& square() {
    static gl::Program prog = utils::builtin_program(gl::Shader(gl::kFragmentShader, files::use_texture_frag()), gl::Shader(gl::kVertexShader, files::transform_shape_vert()));
    return prog;
}

} // namespace programs

} // namespace shaders

#endif // __GENERATED_SHADERS_DEF__
