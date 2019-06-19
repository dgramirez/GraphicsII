#version 450
#extension GL_ARB_separate_shader_objects : enable

//From Vertex Shader
layout(location=0) in vec4 frag_color;

//Out Color
layout(location=0) out vec4 out_color;

void main()
{
	out_color = frag_color;
}