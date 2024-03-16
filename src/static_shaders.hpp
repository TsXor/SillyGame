
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
    static gl::ShaderSource source = utils::builtin_source("transform_shape.vert"s, "#version 330 core\n\nin vec2 pos;\nuniform vec2 rotate;\n\nvoid main() {\n    // Shrink the full screen rectangle to only half size\n    vec2 rotated = vec2(pos.x * rotate.x - pos.y * rotate.y, pos.x * rotate.y + pos.y * rotate.x);\n    gl_Position = vec4(0.5 * rotated.xy, 0, 1);\n}\n"s);
    return source;
}


static inline gl::ShaderSource& cyan_color_frag() {
    static gl::ShaderSource source = utils::builtin_source("cyan_color.frag"s, "#version 330 core\n\nout vec4 fragColor;\n\nvoid main() {\n    fragColor = vec4(0.0, 1.0, 1.0, 1.0);\n}\n"s);
    return source;
}

} // namespace files

namespace programs {

static inline gl::Program& square() {
    static gl::Program prog = utils::builtin_program(gl::Shader(gl::kFragmentShader, files::cyan_color_frag()), gl::Shader(gl::kVertexShader, files::transform_shape_vert()));
    return prog;
}

} // namespace programs

} // namespace shaders

#endif // __GENERATED_SHADERS_DEF__
