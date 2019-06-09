#ifndef VKOBJ_CONTEXT_H
#define VKOBJ_CONTEXT_H

#include "VkObj_WindowSetup.h" 
#include "VkObj_Devices.h" 
#include "VkObj_Swapchain.h"
#include "VkObj_RenderPipeline.h"

class VkObj_Context
{
public:
	VkObj_Context();

	VkObj_WindowProperties window;
	VkObj_DeviceProperties device;
	VkObj_Swapchain swapchain;
	VkObj_RenderPipeline pipelines;

	void shutdown();

	bool init(SDL_Window *win);

	std::vector<VkQueryPool> query_pool;
	bool CreateQueryPool();
	VkCommandPool command_pool;
	std::vector<VkCommandBuffer> command_buffer;
	bool CreateCommandPool();

private:
};

extern VkObj_Context myContext;

#endif