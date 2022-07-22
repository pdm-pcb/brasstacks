#version 460 core

struct Material {
    vec4  ambient;
    vec4  diffuse;
    vec4  specular;
    float shine;
};

layout(std140) uniform CameraMatrixBuffer {
    mat4 view_matrix;
    mat4 proj_matrix;
};

layout(std140) uniform WorldAndMaterial {
    mat4 world_matrix;
    Material material;
};

layout(location = 0) in vec4 in_position;
layout(location = 1) in vec4 in_normal;
layout(location = 2) in vec4 in_tangent;
layout(location = 3) in vec4 in_bitangent;
layout(location = 4) in vec2 in_texcoords;

layout(location = 0) out VS_OUT {
    vec4 world_pos;
    vec3 normal;
    vec3 tangent;
    vec3 bitangent;
    vec2 texcoords;
} vs_out;

void main()
{
    vs_out.world_pos = world_matrix * in_position;
    vs_out.texcoords = in_texcoords;
    vs_out.normal    = normalize(world_matrix * in_normal).xyz;
    vs_out.tangent   = normalize(world_matrix * in_tangent).xyz;
    vs_out.bitangent = normalize(world_matrix * in_bitangent).xyz;

    gl_Position = proj_matrix * view_matrix * world_matrix * in_position;
}