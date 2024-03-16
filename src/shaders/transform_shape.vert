#version 330 core

in vec2 pos;
uniform vec2 rotate;

void main() {
    // rotate vertices
    vec2 rotated = vec2(pos.x * rotate.x - pos.y * rotate.y, pos.x * rotate.y + pos.y * rotate.x);
    // Shrink the full screen rectangle to only half size
    gl_Position = vec4(0.5 * rotated.xy, 0, 1);
}
