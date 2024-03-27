#include <oglwrap/shapes/rectangle_shape.h>

namespace shaders::details {

static inline void square_init(gl::Program& prog) {
    (prog | "pos").bindLocation(gl::RectangleShape::kPosition);
    (prog | "tex_coord").bindLocation(gl::RectangleShape::kTexCoord);
    gl::UniformSampler(prog, "tex") = 0;
}

} // namespace shaders::details
