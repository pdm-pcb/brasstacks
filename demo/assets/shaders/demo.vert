#version 460

layout(location = 0) in vec3 in_pos;

layout(location = 0) out vec3 out_color;

void main() {
    gl_Position = vec4(in_pos, 1.0);
    out_color = in_color;
}
