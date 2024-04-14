// 这个文件是自动生成的，别改。

#ifndef __STATIC_SHADERS_HPP__
#define __STATIC_SHADERS_HPP__

#include "silly_framework/ogl/oglwrap.hpp"

namespace shaders {

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

namespace files {
gl::ShaderSource& transform_shape();
gl::ShaderSource& use_texture();
} // namespace files

namespace programs {
gl::Program& sprite();
} // namespace programs

} // namespace shaders

#endif // __STATIC_SHADERS_HPP__
