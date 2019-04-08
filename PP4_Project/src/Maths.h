#ifndef MATHS_H
#define MATHS_H

#include "Defines.h"

#define POSITION 0
#define COLOR 1

struct Vertex
{
	glm::vec4 position;
	glm::vec4 color;

	static VkVertexInputBindingDescription get_binding_description()
	{
		VkVertexInputBindingDescription vertex_input_binding_description = {};
		
		vertex_input_binding_description.binding = 0;
		vertex_input_binding_description.stride = sizeof(Vertex);
		vertex_input_binding_description.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
		
		return vertex_input_binding_description;
	}

	static std::array<VkVertexInputAttributeDescription, 2> get_attribute_description()
	{
		std::array<VkVertexInputAttributeDescription, 2> attribute_description = {};

		attribute_description[POSITION].binding = 0;
		attribute_description[POSITION].location = 0;
		attribute_description[POSITION].format = VK_FORMAT_R32G32B32A32_SFLOAT;
		attribute_description[POSITION].offset = offsetof(Vertex, position);

		attribute_description[COLOR].binding = 0;
		attribute_description[COLOR].location = 1;
		attribute_description[COLOR].format = VK_FORMAT_R32G32B32A32_SFLOAT;
		attribute_description[COLOR].offset = offsetof(Vertex, color);

		return attribute_description;
	}
};

struct mvp_object
{
	glm::mat4 model;
	glm::mat4 view;
	glm::mat4 projection;
};

//Vulkan is a right hand system, with Y pointing down.
#if 1
const std::vector<Vertex> pyramid = {
	{ { 0.0f, -0.5f,  0.0f, 1.0f}, V_COLOR_RED	  },	//Top point
	{ { 0.5f,  0.5f,  0.5f, 1.0f}, V_COLOR_GREEN  },	//Bottom Vertex 1
	{ {-0.5f,  0.5f,  0.5f, 1.0f}, V_COLOR_BLUE   },	//Bottom Vertex 2
	{ { 0.5f,  0.5f, -0.5f, 1.0f}, V_COLOR_CYAN   },	//Bottom Vertex 3
	{ {-0.5f,  0.5f, -0.5f, 1.0f}, V_COLOR_YELLOW }		//Bottom Vertex 4
};
#else

const std::vector<Vertex> pyramid = {
	{{-0.5f, -0.5f, 0.0f, 1.0f},	V_COLOR_RED},
	{{ 0.5f, -0.5f, 0.0f, 1.0f},	V_COLOR_GREEN},
	{{ 0.5f,  0.5f, 0.0f, 1.0f},	V_COLOR_BLUE},
	{{-0.5f,  0.5f, 0.0f, 1.0f},	V_COLOR_WHITE}
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