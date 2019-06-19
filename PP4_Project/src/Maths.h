#ifndef MATHS_H
#define MATHS_H

#include "pch.h"

#pragma region Unique_Defines
#define V_COLOR_BLACK	{ 0.0f, 0.0f, 0.0f, 1.0f }
#define V_COLOR_RED		{ 1.0f, 0.0f, 0.0f, 1.0f }
#define V_COLOR_GREEN	{ 0.0f, 1.0f, 0.0f, 1.0f }
#define V_COLOR_BLUE	{ 0.0f, 0.0f, 1.0f, 1.0f }
#define V_COLOR_MAGENTA	{ 1.0f, 0.0f, 1.0f, 1.0f }
#define V_COLOR_CYAN	{ 0.0f, 1.0f, 1.0f, 1.0f }
#define V_COLOR_YELLOW	{ 1.0f, 1.0f, 0.0f, 1.0f }
#define V_COLOR_WHITE	{ 1.0f, 1.0f, 1.0f, 1.0f }

#define POSITION 0
#define TANGENT	 1
#define BINORMAL 2
#define NORMAL	 3
#define COLOR	 4
#define UV		 5
#pragma endregion

struct Vertex
{
	glm::vec4 position;
	glm::vec4 tangent;
	glm::vec4 binormal;
	glm::vec4 normal;
	glm::vec4 color;
	glm::vec2 uv;

	static VkVertexInputBindingDescription get_binding_description()
	{
		VkVertexInputBindingDescription vertex_input_binding_description = {};
		
		vertex_input_binding_description.binding = 0;
		vertex_input_binding_description.stride = sizeof(Vertex);
		vertex_input_binding_description.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
		
		return vertex_input_binding_description;
	}

	static std::array<VkVertexInputAttributeDescription, 6> get_attribute_description()
	{
		std::array<VkVertexInputAttributeDescription, 6> attribute_description = {};

		attribute_description[POSITION].binding = 0;
		attribute_description[POSITION].location = POSITION;
		attribute_description[POSITION].format = VK_FORMAT_R32G32B32_SFLOAT;
		attribute_description[POSITION].offset = offsetof(Vertex, position);

		attribute_description[TANGENT].binding = 0;
		attribute_description[TANGENT].location = TANGENT;
		attribute_description[TANGENT].format = VK_FORMAT_R32G32B32_SFLOAT;
		attribute_description[TANGENT].offset = offsetof(Vertex, tangent);

		attribute_description[BINORMAL].binding = 0;
		attribute_description[BINORMAL].location = BINORMAL;
		attribute_description[BINORMAL].format = VK_FORMAT_R32G32B32_SFLOAT;
		attribute_description[BINORMAL].offset = offsetof(Vertex, binormal);

		attribute_description[NORMAL].binding = 0;
		attribute_description[NORMAL].location = NORMAL;
		attribute_description[NORMAL].format = VK_FORMAT_R32G32B32_SFLOAT;
		attribute_description[NORMAL].offset = offsetof(Vertex, normal);

		attribute_description[COLOR].binding = 0;
		attribute_description[COLOR].location = COLOR;
		attribute_description[COLOR].format = VK_FORMAT_R32G32B32_SFLOAT;
		attribute_description[COLOR].offset = offsetof(Vertex, color);

		attribute_description[UV].binding = 0;
		attribute_description[UV].location = UV;
		attribute_description[UV].format = VK_FORMAT_R32G32_SFLOAT;
		attribute_description[UV].offset = offsetof(Vertex, uv);

		return attribute_description;
	}

};

struct FMD_Vertex
{
	glm::vec4 position;
	glm::vec4 color;
	glm::vec4 normal;
	glm::vec4 tangent;
	glm::vec4 binormal;
	glm::vec2 uv;
};

struct Uniform_MVP
{
	glm::mat4 model;
	glm::mat4 view;
	glm::mat4 projection;
};

struct Uniform_Planets
{
	Uniform_MVP mvp;
	glm::mat4 TI_model;
	glm::vec4 light_color = glm::vec4(1.0f, 1.0f, 1.0f, 0.25f); //xyz = color, w = ambient strength
	glm::vec4 sun_pos = glm::vec4(0.0f, 0.0f, 0.0f, 0.75f); //xyz = position, w = specular strength
};

struct UBO_Ships
{						
	Uniform_MVP mvp;
	glm::mat4 TI_modelview;
	glm::vec4 light1_pos = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);		//xyz = position1
	glm::vec4 light1_color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);		//xyz = color1
	glm::vec4 light2_pos = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);		//xyz = position2
	glm::vec4 light2_color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);		//xyz = color2
	glm::vec4 light3_pos = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	glm::vec4 light3_dir = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
	glm::vec4 light3_color = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);
	glm::vec4 strengths = glm::vec4(0.25f, 0.75f, 0.1f, 1.0f);		//x=ambient, y=diffuse, z=attenuation, w=spotlight_ratio
	glm::vec4 cone_strength = glm::vec4(0.01f, 0.1f, 0.0f, 0.0f);
};

struct UBO_Flag
{
	Uniform_MVP mvp;
	glm::vec4 info = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
};

struct Color
{
	union
	{
		unsigned int color;
		unsigned char element[4];
		struct { unsigned char a, r, g, b; };
	};

	Color()
		: color(0x0) {}
	Color(unsigned int _color)
		: color(_color) {}
	Color(unsigned char red, unsigned char green, unsigned char blue, unsigned char alpha)
		: r(red), g(green), b(blue), a(alpha) {}
};

#endif