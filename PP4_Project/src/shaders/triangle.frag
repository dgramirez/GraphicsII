#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(binding=1) uniform sampler2D uv_sampler;

layout(location=0) in vec4 frag_color;
layout(location=1) in vec2 frag_uv;

layout(location=0) out vec4 out_color;

void main()
{
	out_color = texture(uv_sampler, frag_uv);
}