#version 450
#extension GL_ARB_separate_shader_objects : enable

//Binding Texture
layout(binding=1) uniform sampler2D uv_sampler;

//Object's info
layout(location=0) in vec3 frag_pos;			//Position of Fragment Pixel
layout(location=1) in vec2 frag_uv;			//Texture's UVs
layout(location=2) in vec3 frag_viewpos;	//For Specular light
layout(location=3) in vec3 frag_normal;		//Texture's Normals

//Sending External Light Info
layout(location=4) in vec3 frag_light1pos;				//For Diffuse Light
layout(location=5) in vec3 frag_light1color;			//Light Color
layout(location=6) in vec3 frag_light2dir;				//For Directional Light
layout(location=7) in vec3 frag_light2color;			//For Directional Light
layout(location=8) in vec3 frag_light3pos;				//For Spotlight
layout(location=9) in vec3 frag_light3dir;				//For Spotlight
layout(location=10) in vec3 frag_light3color;			//For Spotlight
layout(location=11) in mat4 frag_material;			//Material Data

layout(location=0) out vec4 out_color;

void main()
{
	//Get Object Color
	vec4 object_color = texture(uv_sampler, frag_uv);
	
	//Material Setup
	vec4 ambient_material =  frag_material[0];
	vec3 diffuse_material = vec3(frag_material[1]); 		//w is attenuation
	vec3 specular_material = vec3(frag_material[2]);		//w is shininess
	float shininess = frag_material[2][3];						
	
	//Normal Setup
	vec3 normal = normalize(frag_normal);
	
	//Ambient Light Setup
	vec4 ambient =  ambient_material * object_color;
	//--------------------Light 1--------------------//
	
	//Diffuse Light Setup
	vec3 light1_dir = normalize(frag_light1pos - frag_pos);
	float light1_ratio = max(dot(light1_dir, normal), 0.0f);
	vec4 diffuse = vec4(frag_light1color * (light1_ratio * diffuse_material), 1.0f) * object_color;
		
	//Attenuation by Radius
	float dst = distance(frag_pos, frag_light1pos);
	float diffuse_attenuation_strength = frag_material[1][3];
	float diffuse_attenuation = 1.0f / (1.0f + (diffuse_attenuation_strength * pow(dst, 2)));
	
	//Specular Light Setup	
	vec3 viewdir = normalize(frag_viewpos - frag_pos);
	vec3 half_vec = normalize( (light1_dir) + viewdir );
	float intensity = max(pow(dot(normal, half_vec), shininess), 0.0f);

	vec4 specular = vec4(1.0f, 1.0f, 1.0f, 1.0f) * vec4(intensity * specular_material, 1.0f);
	
	//Total Light for Light 1
	vec4 light1_color = diffuse_attenuation * (diffuse + specular);

	//--------------------Light 2--------------------//
	float light2_ratio = max( dot(-frag_light2dir, normal), 0.0f);
	vec4 light2_color = vec4( frag_light2color * (light2_ratio * diffuse_material), 1.0f) * object_color;
	
	//--------------------Light 3--------------------//
	float frag_light3ratio = frag_material[3][2];
	vec3 light3_dir = normalize( frag_light3pos - frag_pos );
	float surface_ratio = clamp( dot(-light3_dir, frag_light3dir), 0.0f, 1.0f);
	float spot_factor = (surface_ratio > frag_light3ratio ) ? 1.0f: 0.0f;
	float light3_ratio = clamp(dot(light3_dir, normal), 0.0f, 1.0f);
	
	float cone_inner = frag_material[3][0];
	float cone_outer = frag_material[3][1];
	float light3_attenuation = 1.0f - max( (cone_inner - surface_ratio) / (cone_inner - cone_outer), 0.0f);
	
	vec4 light3_color = vec4(light3_attenuation * (frag_light3color  * (light3_ratio * diffuse_material) * spot_factor), 1.0f) * object_color;
	
	//--------------------Total--------------------//
	vec4 total_light_color = ambient + light1_color + light2_color + light3_color;

	//Returning Color
	out_color = total_light_color;
}