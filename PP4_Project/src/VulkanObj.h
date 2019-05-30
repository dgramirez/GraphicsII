#ifndef VULKAN_OBJ2_H
#define VULKAN_OBJ2_H

#include "VulkanObj/VkObj_WindowSetup.h"
#include "VulkanObj/VkObj_Devices.h"
#include "VulkanObj/VkObj_Swapchain.h"
#include "VulkanObj/VkObj_RenderGraphicsPipeline.h"
#include "VulkanObj/VkObj_Texture.h"
#include "VulkanObj/VkObj_Buffers.h"
#include "VulkanObj/VkObj_Command.h"
#include "VulkanObj/VkObj_SemaphoresAndFences.h"
#include "VulkanObj/VkObj_Allocator.h"
#include "VulkanObj/VkObj_StagingBuffer.h"
#include "VulkanObj/VkObj_Context.h"
#include "Object.h"

class VulkanObj
{
public:
	VulkanObj();
	~VulkanObj();

	bool init(const char* title, SDL_Window* window, unsigned short win_width, unsigned short win_height);
	void draw_frames();
	void draw();
	void cleanup();
	void idle_device();
	void reset_swapchain(unsigned short win_width, unsigned short win_height);
	void setup_object_list(uint32_t size);
	void add_to_object_list(const Object3D& object);
	void update(const SDL_Event &e);

private:
	VkObj_SemaphoresAndFences prv_SemaphoreAndFences;
	VkObj_Context context;

	void start_frame();
	void end_frame();

	glm::mat4 myview = glm::translate(glm::mat4(1.0f), glm::vec3(-5.0f, 0.0f, 5.0f));

	uint32_t prv_Frame = 0;
	std::vector<Object3D> prv_ObjectList;

	void CleanupSwapchain();
};

#endif