#ifndef MATHS_H
#define MATHS_H

#include "Defines.h"

#define POSITION 0
#define COLOR 1
#define UV 2

struct Vertex
{
	glm::vec4 position;
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

	static std::array<VkVertexInputAttributeDescription, 3> get_attribute_description()
	{
		std::array<VkVertexInputAttributeDescription, 3> attribute_description = {};

		attribute_description[POSITION].binding = 0;
		attribute_description[POSITION].location = 0;
		attribute_description[POSITION].format = VK_FORMAT_R32G32B32A32_SFLOAT;
		attribute_description[POSITION].offset = offsetof(Vertex, position);

		attribute_description[COLOR].binding = 0;
		attribute_description[COLOR].location = 1;
		attribute_description[COLOR].format = VK_FORMAT_R32G32B32A32_SFLOAT;
		attribute_description[COLOR].offset = offsetof(Vertex, color);

		attribute_description[UV].binding = 0;
		attribute_description[UV].location = 2;
		attribute_description[UV].format = VK_FORMAT_R32G32_SFLOAT;
		attribute_description[UV].offset = offsetof(Vertex, uv);

		return attribute_description;
	}
};

struct Mvp_object
{
	glm::mat4 model;
	glm::mat4 view;
	glm::mat4 projection;
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

//Vulkan is a right hand system, with Y pointing down.
#if 1
const std::vector<Vertex> pyramid = {
	{ { 0.0f, -0.5f,  0.0f, 1.0f}, V_COLOR_RED	 , {0.5f, 0.0f} },	//Top point
	{ { 0.5f,  0.5f,  0.5f, 1.0f}, V_COLOR_GREEN , {1.0f, 1.0f} },	//Bottom Vertex 1
	{ {-0.5f,  0.5f,  0.5f, 1.0f}, V_COLOR_BLUE  , {0.0f, 1.0f} },	//Bottom Vertex 2
	{ { 0.5f,  0.5f, -0.5f, 1.0f}, V_COLOR_CYAN  , {0.0f, 1.0f} },	//Bottom Vertex 3
	{ {-0.5f,  0.5f, -0.5f, 1.0f}, V_COLOR_YELLOW, {1.0f, 1.0f} }	//Bottom Vertex 4
};
#else

const std::vector<Vertex> pyramid = {
	{{-0.5f, -0.5f, 0.0f, 1.0f}, {1.0f, 0.0f, 0.0f, 1.0f}, {1.0f, 0.0f}},
	{{ 0.5f, -0.5f, 0.0f, 1.0f}, {0.0f, 1.0f, 0.0f, 1.0f}, {0.0f, 0.0f}},
	{{ 0.5f,  0.5f, 0.0f, 1.0f}, {0.0f, 0.0f, 1.0f, 1.0f}, {0.0f, 1.0f}},
	{{-0.5f,  0.5f, 0.0f, 1.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {1.0f, 1.0f}}
};

#endif

#if 1
const std::vector<uint32_t> pyramid_indices = {
	0, 2, 4,
	0, 1, 2,
	0, 3, 1,
	0, 4, 3
};
#else
const std::vector<uint32_t> pyramid_indices = {
	0, 1, 2,
	2, 3, 0
};
#endif

#endif