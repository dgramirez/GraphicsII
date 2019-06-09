#version 450
#extension GL_ARB_separate_shader_objects : enable

//Binding Texture
layout(binding=1) uniform sampler2D uv_sampler;

//Object's info
layout(location=0) in vec2 frag_uv;					//Texture's UVs
layout(location=1) in vec3 frag_normal;			//Texture's Normals
layout(location=2) in vec3 frag_pos;				//Position of Fragment Pixel
layout(location=3) in vec3 frag_viewpos;			//Camera Position

//Light Info
layout(location=4) in vec3 frag_lightcolor;			 //Light Color
layout(location=5) in float frag_ambientStrength;	 //For Ambient Light
layout(location=6) in vec3 frag_lightpos;				 //For Diffuse Light
layout(location=7) in float frag_specularStrength;	 //For Specular Light


layout(location=0) out vec4 out_color;

void main()
{
	//Get Object Color
	vec4 object_color = texture(uv_sampler, frag_uv);
	
	//Ambient Light Setup
	vec3 ambient = frag_ambientStrength * frag_lightcolor;
	
	//Diffuse Light Setup
	vec3 normal = normalize(frag_normal);
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