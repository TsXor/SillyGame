#version 330 core

in vec4 vertex;
uniform mat4 pos_trans;
uniform mat4 tex_trans;
out vec2 tex_coord;

void main() {
    gl_Position = pos_trans * vec4(vertex.xy, 0.0, 1.0);
    vec4 tc = tex_trans * vec4(vertex.zw, 0.0, 1.0);
    tex_coord = tc.xy;
}
