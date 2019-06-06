#ifndef VKOBJ_CONTEXT_H
#define VKOBJ_CONTEXT_H

#include "VkObj_WindowSetup.h" 
#include "VkObj_Devices.h" 
#include "VkObj_Swapchain.h"
#include "VkObj_Uniform.h"

class VkObj_Context
{
public:
	VkObj_Context();

	VkObj_WindowProperties window;
	VkObj_DeviceProperties device;
	VkObj_Swapchain swapchain;

	std::vector<VkPipeline> pipelines;
	std::vector<VkPipelineLayout> pipeline_layout;
	
	void shutdown();

	bool init(SDL_Window *win);

	bool CreateQueryPool();
	bool CreateCommandPool();

	std::vector<VkCommandBuffer> command_buffer;
//	bool CreateCommandBuffer(Object3D &object, uint32_t index);

	bool CreatePipelines();
//	bool CreatePipelines(const char* vertex_shader, const char* pixel_shader);

	//	VkObj_Uniform uniform;
	VkDescriptorPool descriptor_pool;
	VkDescriptorSetLayout descriptor_set_layout;
	bool CreateDescriptorPool();
	bool CreateDescriptorSetLayout();

	VkCommandPool command_pool;
	std::vector<VkQueryPool> query_pool;
private:
};

extern VkObj_Context myContext;

#endif