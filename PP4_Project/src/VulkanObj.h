#ifndef VULKAN_OBJ2_H
#define VULKAN_OBJ2_H

#include "VulkanObj/VkObj_WindowSetup.h"
#include "VulkanObj/VkObj_Devices.h"
#include "VulkanObj/VkObj_Pools.h"
#include "VulkanObj/VkObj_Swapchain.h"
#include "VulkanObj/VkObj_RenderGraphicsPipeline.h"
#include "VulkanObj/VkObj_Texture.h"
#include "VulkanObj/VkObj_Buffers.h"
#include "VulkanObj/VkObj_Command.h"
#include "VulkanObj/VkObj_SemaphoresAndFences.h"
#include "VulkanObj/VkObj_Allocator.h"
#include "VulkanObj/VkObj_StagingBuffer.h"

class VulkanObj
{
public:
	VulkanObj();
	~VulkanObj();

	bool init(const char* title, GLFWwindow* window, unsigned short win_width, unsigned short win_height);
	void draw_frames();
	void cleanup();
	void idle_device();
	void reset_swapchain(unsigned short win_width, unsigned short win_height);
	void setup_object_list(uint32_t size);
	void add_to_object_list(const Object3D& object);

private:
	VkObj_WindowProperties prv_Window;
	VkObj_DeviceProperties prv_Devices = VkObj_DeviceProperties(prv_Window.instance, prv_Window.surface);
	VkObj_Pools prv_Pools = VkObj_Pools(prv_Devices.logical, prv_Devices.q_family);
	VkObj_Swapchains prv_Swapchain_old;
	VkObj_Swapchain prv_Swapchain = VkObj_Swapchain(prv_Window, prv_Devices);
	VkObj_RenderGraphicsPipeline prv_RenderGraphicsPipeline;
	VkObj_Texture prv_Texture;
	VkObj_Buffer prv_Buffers_old;
	VkObj_Buffers prv_Buffers;
	VkObj_Commands prv_Command;
	VkObj_SemaphoresAndFences prv_SemaphoreAndFences;
	VkObj_Allocator prv_Allocator;
	VkObj_StagingManager prv_StagingManager;

	uint32_t prv_Frame = 0;
	std::vector<Object3D> prv_ObjectList;

	void CleanupSwapchain();
};

#endif