#version 460 core

layout(std140, binding = 0) uniform CameraMatrixBuffer {
    mat4 view_matrix;
    mat4 proj_matrix;
};

uniform mat4 world_matrix;

layout (location = 0) in vec4 in_position;
layout (location = 1) in vec4 in_color;

out vec4 vs_out_color;

void main()
{
    gl_Position = proj_matrix * view_matrix * world_matrix * in_position;
    vs_out_color = in_color;
}