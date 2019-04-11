#ifndef VULKAN_OBJ2_H
#define VULKAN_OBJ2_H

#include "VulkanObj/VkObj_WindowSetup.h"
#include "VulkanObj/VkObj_ValidationLayer.h"
#include "VulkanObj/VkObj_Devices.h"
#include "VulkanObj/VkObj_Swapchain.h"
#include "VulkanObj/VkObj_RenderGraphicsPipeline.h"
#include "VulkanObj/VkObj_Texture.h"
#include "VulkanObj/VkObj_Buffers.h"
#include "VulkanObj/VkObj_Command.h"
#include "VulkanObj/VkObj_SemaphoresAndFences.h"

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
	VkObj_WindowSetup prv_VkWindow;
	VkObj_ValidationLayer prv_VkValidationLayer;
	VkObj_Devices prv_VkDevices;
	VkObj_Swapchain prv_VkSwapchain;
	VkObj_RenderGraphicsPipeline prv_RenderGraphicsPipeline;
	VkObj_Texture prv_TextureObjects;
	VkObj_Buffers prv_Buffers;
	VkObj_Commands prv_CommandObjects;
	VkObj_SemaphoresAndFences prv_SemaphoreAndFences;

	uint32_t prv_Frame = 0;
	std::vector<Object3D> prv_ObjectList;

	void CleanupSwapchain();
};

#endif