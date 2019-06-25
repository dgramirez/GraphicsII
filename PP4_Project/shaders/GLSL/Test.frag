#version 450
#extension GL_ARB_separate_shader_objects : enable

//Out Color
layout(location=0) out vec4 out_color;

void main()
{
	out_color = vec4(1.0f, 1.0f, 1.0f, 1.0f);
}