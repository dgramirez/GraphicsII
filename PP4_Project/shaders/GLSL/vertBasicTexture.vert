#version 450
#extension GL_ARB_separate_shader_objects : enable

//Uniform
layout(binding = 0) uniform mvp_object {
	mat4 model;
	mat4 view;
	mat4 projection;
	mat4 model_transposed_inversed;
	
	vec3 light_color;
	vec3 sun_pos;
	float ambient_strength;
} mvp;

//Vertex Structure
layout(location=0) in vec3 pos;
layout(location=1) in vec3 color;
layout(location=2) in vec2 uv;
layout(location=3) in vec3 normal;
layout(location=4) in vec3 tangent;

//Sending Vertex info
layout(location=0) out vec2 frag_uv;
layout(location=1) out vec3 frag_normal;
layout(location=2) out vec3 frag_pos;

//Sending External Light Info
layout(location=3) out vec3 frag_light;
layout(location=4) out float frag_ambientStrength;
layout(location=5) out vec3 frag_lightpos;


void main()
{
	//Setup Position
	gl_Position = mvp.projection * mvp.view * mvp.model * vec4(pos, 1.0f);
	frag_pos = vec3(mvp.model * vec4(pos, 1.0f));
	
	//Send Texture UV
	frag_uv = uv;
	
	//Setup Normals
	frag_normal = mat3(mvp.model_transposed_inversed) * normal;
	
	//Send Light Color
	frag_light = mvp.light_color;
	
	//Send Ambient Information
	frag_ambientStrength = mvp.ambient_strength;
	
	//Send Sun Position
	frag_lightpos = mvp.sun_pos;
}