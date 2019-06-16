#version 450
#extension GL_ARB_separate_shader_objects : enable

//Uniform
layout(binding = 0) uniform mvp_object {
	mat4 model;
	mat4 view;
	mat4 projection;
	mat4 model_transposed_inversed;
	vec4 light_color; //xyz = color, w = ambient strength
	vec4 sun_pos; //xyz = pos, w = diffuse strength
} mvp;

//Vertex Structure
layout(location=0) in vec3 pos;
layout(location=1) in vec3 color;
layout(location=2) in vec2 uv;
layout(location=3) in vec3 normal;
layout(location=4) in vec3 tangent;
layout(location=5) in vec3 binormal;

//Sending Vertex info
layout(location=0) out vec2 frag_uv;					//Texture's UVs
layout(location=1) out vec3 frag_normal;				//Texture's Normals
layout(location=2) out vec3 frag_pos;					//Position of Fragment Pixel
layout(location=3) out vec3 frag_viewpos;			//Camera Position

//Sending External Light Info
layout(location=4) out vec3 frag_lightcolor;			 //Light Color
layout(location=5) out float frag_ambientStrength; //For Ambient Light
layout(location=6) out vec3 frag_lightpos;			 //For Diffuse Light
layout(location=7) out float frag_specularStrength; //For Specular Light


void main()
{
	//Setup Position
	gl_Position = mvp.projection * mvp.view * mvp.model * vec4(pos, 1.0f);
	frag_pos = vec3(mvp.model * vec4(pos, 1.0f));
	frag_viewpos = vec3(mvp.view * vec4(1.0f, 1.0f, 1.0f, 1.0f));
	
	//Send Texture UV
	frag_uv = uv;
	
	//Setup Normals
	frag_normal = mat3(mvp.model_transposed_inversed) * normal;
	
	//Send Light Color
	frag_lightcolor = vec3(mvp.light_color);
	
	//Send Sun Position
	frag_lightpos = vec3(mvp.sun_pos);
	
	//Send Ambient Information
	frag_ambientStrength = mvp.light_color.w;
	
	//Send Specular Information
	frag_specularStrength = mvp.sun_pos.w;
}