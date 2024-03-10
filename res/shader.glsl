#version 330 core
in vec2 pos;
void main() {
    // Shrink the full screen rectangle to only half size
    gl_Position = vec4(0.5 * pos.xy, 0, 1);
}