#include <oglwrap/shapes/rectangle_shape.h>

namespace shaders::details {

static inline void sprite_init(gl::Program& prog) {
    (prog | "vertex").bindLocation(0);
    gl::UniformSampler(prog, "tex") = 0;
}

} // namespace shaders::details
