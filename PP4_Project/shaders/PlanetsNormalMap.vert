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
layout(location=0) in vec4 position;
layout(location=1) in vec4 tangent;
layout(location=2) in vec4 binormal;
layout(location=3) in vec4 normal;
layout(location=4) in vec4 color;
layout(location=5) in vec2 uv;

//Sending Vertex info
layout(location=0) out vec2 frag_uv;					//Texture's UVs
layout(location=1) out mat3 frag_TBN;					//Texture's Normals
layout(location=4) out vec3 frag_pos;					//Position of Fragment Pixel
layout(location=5) out vec3 frag_viewpos;			//Camera Position

//Sending External Light Info
layout(location=6) out vec3 frag_lightcolor;			 //Light Color
layout(location=7) out float frag_ambientStrength; //For Ambient Light
layout(location=8) out vec3 frag_lightpos;			 //For Diffuse Light
layout(location=9) out float frag_specularStrength; //For Specular Light


void main()
{
	//Setup TBN
	vec3 T =	normalize(mat3(mvp.model) * tangent.xyz);
	vec3 N =	normalize(mat3(mvp.model) * normal.xyz);
	
	T = normalize(T - dot(T,N) * N);
	
	vec3 B = 	cross(N,T);
	
	frag_TBN = transpose(inverse(mat3(T, B, N)));
	
	//Setup Position
	gl_Position = mvp.projection * mvp.view * mvp.model * position;
	frag_pos = vec3(mvp.model * position);
	frag_viewpos = vec3(mvp.view[3]);
	
	//Send Texture UV
	frag_uv = uv;
	
	//Send Light Color
	frag_lightcolor = vec3(mvp.light_color);
	
	//Send Sun Position
	frag_lightpos = frag_TBN * vec3(mvp.sun_pos);
	
	//Send Ambient Information
	frag_ambientStrength = mvp.light_color.w;
	
	//Send Specular Information
	frag_specularStrength = mvp.sun_pos.w;
}