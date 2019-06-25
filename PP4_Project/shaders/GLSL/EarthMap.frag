#version 450
#extension GL_ARB_separate_shader_objects : enable

//Binding Texture
layout(binding=1) uniform sampler2D uv_sampler;
layout(binding=2) uniform sampler2D normal_map;
layout(binding=3) uniform sampler2D ambient_map;
layout(binding=4) uniform sampler2D specular_map;

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
	
	vec3 normal = TBN * new_normal;
	
//	vec3 normal = normalize(frag_TBN[2]);
	
	//Light Color
	vec3 earth_lights = vec3(texture(ambient_map, frag_uv));
	
	//Ambient Light Setup
	vec4 ambient = vec4(earth_lights, 1.0f) * object_color;
	
	//Diffuse Light Setup
	vec3 light_dir = normalize(frag_lightpos - frag_pos);
	float light_ratio = clamp(dot(light_dir, normal), 0.0f, 1.0f);
	vec4 diffuse = vec4(light_ratio * frag_lightcolor, 1.0f) * object_color;
	
	//Point Light Addition
//	vec3 point_light = frag_lightcolor * earth_lights;
	
	//Specular Light Setup
	vec3 viewdir = normalize(frag_viewpos - frag_pos);
	vec3 half_vec = normalize( (light_dir) + viewdir );
	float intensity = max(pow(dot(normal, half_vec),32),0.0f);
	vec3 specular = vec3(1.0f, 1.0f, 1.0f) * frag_specularStrength * intensity;
	
//	vec3 reflection = reflect(-light_dir, normal);
//	float shiningness = pow(clamp(dot(viewdir,reflection),0.0f, 1.0f),32);
//	vec3 specular = vec3(1.0f, 1.0f, 1.0f) * shiningness * vec3(texture(specular_map, frag_uv));
	
	//Total Light
//	vec3 light_color = ambient + diffuse;
	vec4 light_color = ambient + diffuse + (diffuse * texture(ambient_map, frag_uv)) + vec4(specular,1.0f);

	//Returning Color
	out_color = light_color;
}