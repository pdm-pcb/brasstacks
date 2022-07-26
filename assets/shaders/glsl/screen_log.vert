#version 460 core

layout(std140) uniform CameraMatrixBuffer {
    mat4 view_matrix;
    mat4 proj_matrix;
};

layout (location = 0) in vec4 in_position;

out vec2 vs_out_texcoord;

void main() {
    vs_out_texcoord = in_position.zw;
    gl_Position = proj_matrix * vec4(in_position.xy, 0.0, 1.0);
}