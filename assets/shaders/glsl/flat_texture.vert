#version 460 core

layout(std140) uniform CameraMatrixBuffer {
    mat4 view_matrix;
    mat4 proj_matrix;
};

uniform mat4 world_matrix;

layout (location = 0) in vec4 in_position;
layout (location = 1) in vec2 in_texcoord;

out vec2 vs_out_texcoord;

void main() {
    vs_out_texcoord = in_texcoord;
    gl_Position = proj_matrix * view_matrix * world_matrix * in_position;
}