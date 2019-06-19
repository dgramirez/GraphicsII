#version 450
#extension GL_ARB_separate_shader_objects : enable

//Binding Texture
layout(binding=1) uniform sampler2D uv_sampler;
layout(binding=2) uniform sampler2D normal_map;

//Object's info
//Sending Vertex info
layout(location=0) in vec2 frag_uv;					//Texture's UVs
layout(location=1) in mat3 frag_TBN;				//Texture's Normals
layout(location=4) in vec3 frag_pos;					//Position of Fragment Pixel
layout(location=5) in vec3 frag_viewpos;			//Camera Position

//Sending External Light Info
layout(location=6) in vec3 frag_lightcolor;			 //Light Color
layout(location=7) in float frag_ambientStrength; //For Ambient Light
layout(location=8) in vec3 frag_lightpos;			 //For Diffuse Light
layout(location=9) in float frag_specularStrength; //For Specular Light


layout(location=0) out vec4 out_color;

void main()
{
	//Get Object Color
	vec4 object_color = texture(uv_sampler, frag_uv);
	vec3 normal_map = vec3(texture(normal_map, frag_uv));

	//Normals Setup
	vec3 new_normal = normalize(normal_map * 2.0f - 1.0f);
	
	mat3 TBN = frag_TBN;
	TBN[0] = normalize(TBN[0]);
	TBN[1] = normalize(TBN[1]);
	TBN[2] = normalize(TBN[2]);
	
	vec3 normal = new_normal * TBN;
	
//	vec3 normal = normalize(frag_TBN[2]);
	
	
	
	//Ambient Light Setup
	vec3 ambient = frag_ambientStrength * frag_lightcolor;
	
	//Diffuse Light Setup
	vec3 direction = normalize(frag_lightpos - frag_pos);
	float diffuse_strength = clamp(dot(normal, direction), 0.0f, 1.0f);
	vec3 diffuse = diffuse_strength * frag_lightcolor;
	
	//Specular Light Setup
//	vec3 view = normalize(frag_viewpos - frag_pos);
//	vec3 reflection = reflect(-direction, normal);
//	float shiningness = pow(max(dot(view,reflection),0.0f),256);
//	vec3 specular = frag_specularStrength * shiningness * light_color;
	
	//Total Light
	vec3 light_color = ambient + diffuse;
//	vec3 light_color = ambient + diffuse + specular;

	//Returning Color
	out_color = vec4(light_color,1.0f) * object_color;
}