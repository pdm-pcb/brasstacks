#version 460 core

uniform sampler2D text;
uniform vec4 text_color;

in  vec2 vs_out_texcoord;
out vec4 final_color;

void main() {
    vec4 texel = vec4(1.0, 1.0, 1.0, texture(text, vs_out_texcoord).r);
	final_color = text_color * texel;
}
