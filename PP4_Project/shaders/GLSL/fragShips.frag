#version 450
#extension GL_ARB_separate_shader_objects : enable

//Binding Texture
layout(binding=1) uniform sampler2D uv_sampler;

//Object's info
layout(location=0) in vec2 frag_uv;					//Texture's UVs
layout(location=1) in vec3 frag_normal;			//Texture's Normals
layout(location=2) in vec3 frag_pos;				//Position of Fragment Pixel

//Light Info
layout(location=3) in vec3 frag_light1pos;			//For Diffuse Light
layout(location=4) in vec3 frag_light2dir;				//For Directional Light
layout(location=5) in vec3 frag_light1color;			//Light Color
layout(location=6) in vec3 frag_light2color;			//For Directional Light
layout(location=7) in float frag_ambientStrength;	//For Ambient Light
layout(location=8) in float frag_specularStrength;	//For Specular Light
layout(location=9) in float frag_attenuation;			//For Attenuation


layout(location=0) out vec4 out_color;

void main()
{
	//Get Object Color
	vec4 object_color = texture(uv_sampler, frag_uv);
	
	//Ambient Light Setup
	vec3 ambient = frag_ambientStrength * frag_light1color;
	
	//--------------------Light 1--------------------//
	
	//Diffuse Light Setup
	vec3 normal = normalize(frag_normal);
	vec3 light_dir = normalize(frag_light1pos - frag_pos);
	float diffuse_strength = clamp(dot(normal, light_dir), 0.0f, 1.0f);
	vec3 diffuse = diffuse_strength * frag_light1color;
	
	//Specular Light Setup
	vec3 view_dir = normalize(-frag_pos);
	vec3 reflection = reflect(-light_dir, normal);
	float shiningness = pow(clamp(dot(view_dir,reflection),0.0f, 1.0f),32);
	vec3 specular = frag_specularStrength * shiningness * frag_light1color;
	
	//Attenuation
	float dst = distance(frag_pos, frag_light1pos);
	float attenuation = 1.0f / (1.0f + (frag_attenuation * pow(dst, 2)));
	
	//Total Light for Light 1
	vec3 light1_color = attenuation * diffuse;
//	vec3 light1_color = ambient + (attenuation * (diffuse + specular));

	//--------------------Light 2--------------------//
	float light2_ratio = clamp( dot(-frag_light2dir, normal), 0.0f, 1.0f );
	vec3 light2_color = light2_ratio * frag_light2color;
	
	//--------------------Total--------------------//
	vec3 total_light_color = ambient + light1_color + light2_color;

	//Returning Color
	out_color = vec4(total_light_color,1.0f) * object_color;
}