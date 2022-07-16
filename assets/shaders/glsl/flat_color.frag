#version 460 core

in  vec4 vs_out_color;
out vec4 final_color;

void main()
{
	final_color = vs_out_color;
}
