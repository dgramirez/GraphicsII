#ifndef VKOBJ_CONTEXT_H
#define VKOBJ_CONTEXT_H

#include "VkObj_WindowSetup.h" 
#include "VkObj_Devices.h" 
#include "VkObj_Swapchain.h"
#include "VkObj_Uniform.h"

class Object3D;

class VkObj_Context
{
public:
	VkObj_Context();

	VkObj_WindowProperties window;
	VkObj_DeviceProperties device;
	VkObj_Swapchain swapchain;
	VkObj_Uniform uniform;
	std::vector<VkPipeline> pipelines;
	std::vector<VkPipelineLayout> pipeline_layout;
	std::vector<Object3D> *pObjectList;

	void shutdown();

	bool init(GLFWwindow *win, std::vector<Object3D> &object_list);

	bool CreateQueryPool();
	bool CreateCommandPool();

	std::vector<VkCommandBuffer> command_buffer;
	bool CreateCommandBuffer(Object3D &object, uint32_t index);

	bool CreatePipelines();
//	bool CreatePipelines(const char* vertex_shader, const char* pixel_shader);

	VkCommandPool command_pool;
	std::vector<VkQueryPool> query_pool;
private:
};

#endif