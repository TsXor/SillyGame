#version 330 core

in vec2 tex_coord;
uniform sampler2D tex;
out vec4 frag_color;

void main() {
    frag_color = texture(tex, vec2(tex_coord.x, 1 - tex_coord.y));
}
