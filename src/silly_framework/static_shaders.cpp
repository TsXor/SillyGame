// 这个文件是自动生成的，别改。

#include "./static_shaders.hpp"
#include "./static_shader_inits.hpp"

using namespace std::string_literals;

namespace utils {

static inline gl::ShaderSource make_source(const std::string& name, const std::string& text) {
    gl::ShaderSource source;
    source.set_source(text);
    source.set_source_file(name);
    return source;
}

template <typename... Shaders>
static inline gl::Program make_program(Shaders&&... shaders) {
    gl::Program prog;
    (..., prog.attachShader(shaders));
    prog.link();
    return prog;
}

template <typename... OpTs> requires (... && std::is_invocable_v<OpTs, gl::Program&>)
static inline gl::Program&& init_program(gl::Program&& prog, OpTs... ops) {
    (..., ops(prog));
    return std::move(prog);
}

} // namespace utils


gl::ShaderSource& shaders::files::use_texture() {
    static gl::ShaderSource source = utils::make_source(
        "use_texture.frag"s,
        "#version 330 core\n\nin vec2 tex_coord;\nuniform sampler2D tex;\nout vec4 frag_color;\n\nvoid main() {\n    frag_color = texture(tex, vec2(tex_coord.x, 1 - tex_coord.y));\n}\n"s
    );
    return source;
}
gl::ShaderSource& shaders::files::transform_shape() {
    static gl::ShaderSource source = utils::make_source(
        "transform_shape.vert"s,
        "#version 330 core\n\nin vec4 vertex;\nuniform mat4 pos_trans;\nuniform mat4 tex_trans;\nout vec2 tex_coord;\n\nvoid main() {\n    gl_Position = pos_trans * vec4(vertex.xy, 0.0, 1.0);\n    vec4 tc = tex_trans * vec4(vertex.zw, 0.0, 1.0);\n    tex_coord = tc.xy;\n}\n"s
    );
    return source;
}

gl::Program& shaders::programs::sprite() {
    static gl::Program prog = utils::init_program(
        utils::make_program(
            gl::Shader(gl::kFragmentShader, files::use_texture()), 
            gl::Shader(gl::kVertexShader, files::transform_shape())
        ),
        ::shaders::details::sprite_init
    );
    return prog;
}
