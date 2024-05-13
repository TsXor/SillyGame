#version 330 core

in vec4 vertex;
uniform mat4 pos_trans;

void main() {
    gl_Position = pos_trans * vec4(vertex.xy, 0.0, 1.0);
}
