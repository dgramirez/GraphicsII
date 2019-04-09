#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 0) uniform mvp_object {
	mat4 model;
	mat4 view;
	mat4 projection;
} mvp;

layout(location=0) in vec4 pos;
layout(location=1) in vec4 color;
layout(location=2) in vec2 uv;

layout(location=0) out vec4 frag_color;
layout(location=1) out vec2 frag_uv;

void main()
{
	gl_Position = mvp.projection * mvp.view * mvp.model * pos;
	frag_color = color;
	frag_uv = uv;
}