#version 450
#extension GL_ARB_separate_shader_objects : enable

//Binding Texture
layout(binding=1) uniform sampler2D uv_sampler;

//Object's info
layout(location=0) in vec2 frag_uv;					//Texture's UVs

//Return Value
layout(location=0) out vec4 out_color;

void main()
{
	//Get Object Color
	vec4 object_color = texture(uv_sampler, frag_uv);
	out_color = object_color;
}