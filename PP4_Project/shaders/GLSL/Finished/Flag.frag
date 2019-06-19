#version 450
#extension GL_ARB_separate_shader_objects : enable

//Binding Texture
layout(binding=1) uniform sampler2D uv_sampler;

//From Vertex Shader
layout(location=0) in vec2 frag_uv;

//Out Color
layout(location=0) out vec4 out_color;

void main()
{
	out_color = texture(uv_sampler, frag_uv);
}