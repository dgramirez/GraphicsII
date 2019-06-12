#version 450
#extension GL_ARB_separate_shader_objects : enable

//Uniform
layout(binding = 0) uniform UBO_Ships {
	mat4 model;
	mat4 view;
	mat4 projection;
	mat4 TI_modelview;
	vec4 light1_pos;
	vec4 light1_color;
	vec4 light2_dir;
	vec4 light2_color;
	vec4 strengths;
} ubo;

//Vertex Structure
layout(location=0) in vec3 pos;
layout(location=1) in vec3 color;
layout(location=2) in vec2 uv;
layout(location=3) in vec3 normal;
layout(location=4) in vec3 tangent;

//Sending Vertex info
layout(location=0) out vec2 frag_uv;			//Texture's UVs
layout(location=1) out vec3 frag_normal;		//Texture's Normals
layout(location=2) out vec3 frag_pos;			//Position of Fragment Pixel

//Sending External Light Info
layout(location=3) out vec3 frag_light1pos;				//For Diffuse Light
layout(location=4) out vec3 frag_light2dir;				//For Directional Light
layout(location=5) out vec3 frag_light1color;			//Light Color
layout(location=6) out vec3 frag_light2color;			//For Directional Light
layout(location=7) out float frag_ambientStrength;	//For Ambient Light
layout(location=8) out float frag_specularStrength;	//For Specular Light
layout(location=9) out float frag_attenuation;			//For Attenuation



void main()
{
	//Setup Position
	gl_Position = ubo.projection * ubo.view * ubo.model * vec4(pos, 1.0f);
	frag_pos = vec3(ubo.view * ubo.model * vec4(pos, 1.0f));
	
	//Send Texture UV
	frag_uv = uv;
	
	//Setup Normals
	frag_normal = mat3(ubo.TI_modelview) * normal;
	
	//Send Light Position
	frag_light1pos = vec3(ubo.view * ubo.light1_pos);
	frag_light2dir = vec3(ubo.view * ubo.light2_dir);
	
	//Send Light Color
	frag_light1color = vec3(ubo.light1_color);
	frag_light2color = vec3(ubo.light2_color);
	
	//Send Ambient Information
	frag_ambientStrength = ubo.strengths.x;
	
	//Send Specular Information
	frag_specularStrength = ubo.strengths.y;
	
	//Send Attenuation Information
	frag_attenuation = ubo.strengths.z;
}