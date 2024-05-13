// 这个文件是自动生成的，别改。

#ifndef __STATIC_SHADERS_HPP__
#define __STATIC_SHADERS_HPP__

#include "silly_framework/bundled/glad/glad.h"
#include <oglwrap/oglwrap.h>

namespace shaders {

namespace files {
gl::ShaderSource& use_color();
gl::ShaderSource& transform_shape();
gl::ShaderSource& use_texture();
gl::ShaderSource& transform_shape_notex();
} // namespace files

namespace programs {
gl::Program& fill();
gl::Program& sprite();
} // namespace programs

} // namespace shaders

#endif // __STATIC_SHADERS_HPP__
