#version 460 core

uniform sampler2D tex_sampler;

in  vec2 vs_out_texcoord;
out vec4 final_color;

void main() {
	final_color = texture(tex_sampler, vs_out_texcoord);
}
