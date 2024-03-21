#version 330 core

in vec2 trans_tex_coord;
uniform sampler2D tex;
out vec4 frag_color;

void main() {
    frag_color = texture(tex, vec2(trans_tex_coord.x, 1 - trans_tex_coord.y));
}
