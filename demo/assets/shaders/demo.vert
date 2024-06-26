#version 460

layout(location = 0) in vec3 in_pos;
layout(location = 1) in vec3 in_color;
layout(location = 2) in vec2 in_uv;

layout(set = 0, binding = 0) uniform CameraData {
    mat4 view_mat;
    mat4 proj_mat;
};

layout(push_constant) uniform vert_constants {
	mat4 model_mat;
};

layout(location = 0) out vec3 out_color;
layout(location = 1) out vec2 out_uv;

void main() {
    gl_Position = proj_mat * view_mat * model_mat * vec4(in_pos, 1.0);
    out_color = in_color;
    out_uv = in_uv;
}
