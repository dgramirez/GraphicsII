#version 450
#extension GL_ARB_separate_shader_objects : enable

//Uniform
layout(binding = 0) uniform ubo {
	mat4 model;
	mat4 view;
	mat4 projection;
} mvp;

//Vertex Structure
layout(location=0) in vec4 position;
layout(location=1) in vec4 tangent;
layout(location=2) in vec4 binormal;
layout(location=3) in vec4 normal;
layout(location=4) in vec4 color;
layout(location=5) in vec2 uv;

//Out to the fragment shader
layout(location=0) out vec4 frag_color;

void main()
{
	gl_Position = mvp.projection * mvp.view * mvp.model * position;
	frag_color = color;
}