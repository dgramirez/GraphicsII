#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 0) uniform mvp_object {
	mat4 model;
	mat4 view;
	mat4 projection;
	vec4 time;
} ubo;

layout(location=0) in vec4 pos;
layout(location=1) in vec4 color;
layout(location=2) in vec2 uv;
layout(location=3) in vec3 normal;
layout(location=4) in vec3 tangent;
layout(location=5) in vec3 binormal;

layout(location=0) out vec2 frag_uv;

void main()
{
	float time = ubo.time.x;
	float angle = mod(time, 360.0f);

	vec4 newpos = pos;
	newpos.y += sin(newpos.x + angle);
	newpos.y += sin(newpos.z * 0.5f + angle);
	newpos.z += sin( (newpos.y - 3.0f) + (angle * 0.5f) );
	
	gl_Position = ubo.projection * ubo.view * ubo.model * newpos;
	frag_uv = uv;
}