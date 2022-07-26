#version 460 core

uniform sampler2D text;

in  vec2 vs_out_texcoord;
out vec4 final_color;

void main() {
	float texel = texture(text, vs_out_texcoord).r;
    final_color = vec4(1.0, 1.0, 1.0, texel);
}
