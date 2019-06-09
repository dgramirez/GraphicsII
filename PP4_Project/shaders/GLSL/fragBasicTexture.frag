#version 450
#extension GL_ARB_separate_shader_objects : enable

//Binding Texture
layout(binding=1) uniform sampler2D uv_sampler;

//Vertex Shader Input
layout(location=0) in vec2 frag_uv;
layout(location=1) in vec3 frag_normal;
layout(location=2) in vec3 frag_pos;
layout(location=3) in vec3 frag_light;
layout(location=4) in float frag_ambientStrength;
layout(location=5) in vec3 frag_lightpos;

layout(location=0) out vec4 out_color;

void main()
{
	//Get Object Color
	vec4 object_color = texture(uv_sampler, frag_uv);
	
	//Ambient Light Setup
	vec3 ambient = frag_ambientStrength * frag_light;
	
	//Diffuse Light Setup
	vec3 norm = normalize(frag_normal);
	vec3 light_dir = normalize(frag_lightpos - frag_pos);
	float diff = max(dot(norm, light_dir), 0.0f);
	vec3 diffuse = diff * frag_light;
	
	//Total Light
	vec4 light_color = vec4(ambient + diffuse, 1.0f);
	
	//Returning Color
	out_color = light_color * object_color;
}