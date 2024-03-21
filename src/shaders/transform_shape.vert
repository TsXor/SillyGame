#version 330 core

in vec2 pos;
in vec2 tex_coord;
uniform vec2 rotate;
out vec2 trans_tex_coord;

void main() {
    // rotate vertices
    vec2 rotated_pos = vec2(pos.x * rotate.x - pos.y * rotate.y, pos.x * rotate.y + pos.y * rotate.x);
    // shrink the full screen rectangle to only half size
    gl_Position = vec4(0.5 * rotated_pos.xy, 0, 1);
    // pass tex_coord
    trans_tex_coord = tex_coord;
}
