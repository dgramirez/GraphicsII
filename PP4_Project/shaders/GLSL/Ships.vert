#version 450
#extension GL_ARB_separate_shader_objects : enable

//Uniform
layout(binding = 0) uniform UBO_Ships {
	mat4 model;
	mat4 view;
	mat4 projection;
	mat4 TI_model;
	vec4 light1_pos;
	vec4 light1_color;
	vec4 light2_dir;
	vec4 light2_color;
	vec4 light3_pos;
	vec4 light3_dir;
	vec4 light3_color;
	mat4 material;
} ubo;

//Vertex Structure
layout(location=0) in vec4 position;
layout(location=1) in vec4 tangent;
layout(location=2) in vec4 binormal;
layout(location=3) in vec4 normal;
layout(location=4) in vec4 color;
layout(location=5) in vec2 uv;

//Sending Vertex info
layout(location=0) out vec3 frag_pos;			//Position of Fragment Pixel
layout(location=1) out vec2 frag_uv;			//Texture's UVs
layout(location=2) out vec3 frag_viewpos;	//For Specular light
layout(location=3) out vec3 frag_normal;		//Texture's Normals

//Sending External Light Info
layout(location=4) out vec3 frag_light1pos;				//For Diffuse Light
layout(location=5) out vec3 frag_light1color;			//Light Color
layout(location=6) out vec3 frag_light2dir;				//For Directional Light
layout(location=7) out vec3 frag_light2color;			//For Directional Light
layout(location=8) out vec3 frag_light3pos;				//For Spotlight
layout(location=9) out vec3 frag_light3dir;				//For Spotlight
layout(location=10) out vec3 frag_light3color;			//For Spotlight
layout(location=11) out mat4 frag_material;				//Material Data

void main()
{
	//Setup Position
	gl_Position = ubo.projection * ubo.view * ubo.model * position;
	frag_pos = vec3(ubo.model * position);
	
	//Send Texture UV
	frag_uv = uv;
	
	//Send View Position
	mat4 inv_view = inverse(ubo.view);
	frag_viewpos = vec3(inv_view[3].xyz);

	//Setup Normals
	frag_normal = mat3(ubo.TI_model) * vec3(normal);
	
	//Point Light Info
	frag_light1pos = vec3(vec4(ubo.light1_pos.xyz, 1.0f));
	frag_light1color = vec3(ubo.light1_color);
	
	//Directional Light Info
	frag_light2dir = vec3(ubo.light2_dir);
	frag_light2color = vec3(ubo.light2_color);
	
	//Spot Light Info
	frag_light3pos = vec3(vec4(ubo.light3_pos.xyz, 1.0f));
	frag_light3dir = vec3(ubo.light3_dir);
	frag_light3color = vec3(ubo.light3_color);
	
	//Material Info
	frag_material = ubo.material;
}